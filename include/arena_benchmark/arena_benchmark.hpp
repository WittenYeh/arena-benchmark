#pragma once

// ArenaBenchmark: Google Benchmark-style runner with built-in result visualization.
//
// Usage example:
//
//   int main(int argc, char** argv) {
//       using namespace arena_benchmark;
//       ArenaBenchmark bench;
//
//       std::vector<int> data = make_data(1000);
//
//       bench.register_benchmark("BM_StdSort",
//           [&data](benchmark::State& state) {
//               for (auto _ : state) {
//                   auto copy = data;
//                   std::sort(copy.begin(), copy.end());
//                   benchmark::DoNotOptimize(copy);
//               }
//           })
//           .repetitions(5)
//           .extra_info("std::sort on 1K ints");
//
//       bench.warm_up(1).run_all(argc, argv).export_results("output");
//   }

#include <functional>
#include <memory>
#include <cassert>
#include <string>
#include <map>
#include <list>
#include <optional>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <benchmark/benchmark.h>

#include <arena_benchmark/arena_reporter.hpp>
#include <arena_benchmark/instance_registration.hpp>
#include <arena_benchmark/repetition_log_list.hpp>
#include <arena_benchmark/repetition_log_list_visualizer.hpp>
#include <arena_benchmark/result_exporter.hpp>
#include <arena_benchmark/summary_table.hpp>
#include <arena_benchmark/summary_table_visualizer.hpp>

namespace arena_benchmark {

class ArenaBenchmark {
public:
    ArenaBenchmark() = default;

    // Register a benchmark.
    //
    //   bench.register_benchmark("BM_Sort",
    //       [](benchmark::State& state) { ... })
    //       .repetitions(3)
    //       .extra_info("std::sort");
    template<typename Func>
    auto register_benchmark(const std::string& name, Func&& func)
        -> InstanceRegistration& {
        auto duplicated = std::find_if(_instances.begin(), _instances.end(),
            [&](const InstanceRegistration& inst) { return inst.name() == name; });
        if (duplicated != _instances.end()) {
            throw std::invalid_argument("Duplicate benchmark name: " + name);
        }
        _instances.emplace_back(
            name,
            std::function<void(benchmark::State&)>(std::forward<Func>(func))
        );
        return _instances.back();
    }

    // Set warm-up repetitions to run before measured repetitions.
    // Warm-up rows are shown in Repetition Log Board but excluded from Summary Board.
    auto warm_up(int num_warm_up_reps) -> ArenaBenchmark& {
        if (num_warm_up_reps < 0) {
            throw std::invalid_argument("warm_up repetitions must be >= 0");
        }
        _num_warm_up_reps = num_warm_up_reps;
        return *this;
    }

    // Run all benchmarks (uses dummy argc/argv)
    auto run_all() -> ArenaBenchmark& {
        int argc = 1;
        const char* name = "arena_benchmark";
        char* argv[] = {const_cast<char*>(name), nullptr};
        return run_all(argc, argv);
    }

    // Run all benchmarks with user-provided argc/argv.
    auto run_all(int& argc, char** argv) -> ArenaBenchmark& {
        benchmark::Initialize(&argc, argv);

        // Auto-size columns based on registered benchmark data
        RepetitionLogListVisualizer::auto_size_columns(_instances);
        SummaryTableVisualizer::auto_size_columns(_instances);

        ArenaReporter reporter;
        RepetitionLogList log_list;
        std::map<std::string, std::vector<SingleRepetitionResult>> results_map;

        for (auto& inst : _instances) {
            auto fn = inst.func();

            benchmark::ClearRegisteredBenchmarks();
            benchmark::RegisterBenchmark(inst.name().c_str(),
                [fn](benchmark::State& state) {
                    fn(state);
                })->Repetitions(1)->Unit(inst.time_unit());

            auto run_phase = [&](int rep, bool is_warm_up) {
                reporter.clear_results();
                benchmark::RunSpecifiedBenchmarks(&reporter);
                for (auto result : reporter.get_results()) {
                    result.instance_name(inst.name())
                          .extra_info(inst.extra_info_str())
                          .repetition_index(rep)
                          .is_warm_up(is_warm_up)
                          .compute_avg_time_per_item(inst.workload_scale())
                          .compute_items_per_second(inst.workload_scale());
                    log_list << result;
                    if (!is_warm_up) {
                        results_map[inst.name()].push_back(result);
                    }
                }
            };

            for (int rep = 0; rep < _num_warm_up_reps; ++rep) {
                run_phase(rep, true);
            }

            for (int rep = 0; rep < inst.num_repetitions(); ++rep) {
                run_phase(rep, false);
            }
        }

        _last_log_list = std::move(log_list);
        _last_summary_table = _build_summary_table(results_map);

        std::cout << std::endl << *_last_summary_table << std::endl;
        return *this;
    }

    // Export the latest benchmark run to JSON files.
    // Output layout:
    //   <output_root>/benchmark_logs/benchmark_repetition_log_board.json
    //   <output_root>/benchmark_results/benchmark_summary_board.json
    auto export_results(const std::filesystem::path& output_root = ".") -> ArenaBenchmark& {
        _ensure_results_ready();

        ResultExporter exporter(output_root / "benchmark_logs",
                                output_root / "benchmark_results");
        exporter.write_repetition_log_board(*_last_log_list);
        exporter.write_summary_board(*_last_summary_table);
        return *this;
    }

private:
    std::list<InstanceRegistration> _instances;
    int _num_warm_up_reps = 0;
    std::optional<RepetitionLogList> _last_log_list;
    std::optional<SummaryTable> _last_summary_table;

    auto _ensure_results_ready() const -> void {
        if (!_last_log_list.has_value() || !_last_summary_table.has_value()) {
            throw std::runtime_error("No benchmark results available. Call run_all() before export_results().");
        }
    }

    auto _build_summary_table(
        const std::map<std::string, std::vector<SingleRepetitionResult>>& results_map) const -> SummaryTable {

        SummaryTable summary_table;
        summary_table.set_sort_by_key("avg_real_time");

        for (auto& [bm_name, results] : results_map) {
            if (results.empty()) continue;

            const InstanceRegistration* matched_inst = nullptr;
            for (auto& inst : _instances) {
                if (inst.name() == bm_name) {
                    matched_inst = &inst;
                    break;
                }
            }
            if (matched_inst == nullptr) {
                throw std::runtime_error("Internal error: benchmark instance not found for " + bm_name);
            }

            double avg_real = 0, avg_cpu = 0, avg_items = 0;
            for (auto& r : results) {
                avg_real  += r.real_time();
                avg_cpu   += r.cpu_time();
                avg_items += static_cast<double>(r.items_per_second());
            }
            int n = static_cast<int>(results.size());
            avg_real  /= n;
            avg_cpu   /= n;
            avg_items /= n;
            const double avg_time_per_item =
                avg_real / static_cast<double>(matched_inst->workload_scale());

            MultiRepetitionSummary summary;
            summary.instance_name(matched_inst->name())
                   .repetitions(n)
                   .avg_real_time(avg_real)
                   .avg_cpu_time(avg_cpu)
                   .avg_time_per_item(avg_time_per_item)
                   .avg_items_per_second(avg_items)
                   .time_unit(results[0].time_unit())
                   .extra_info(matched_inst->extra_info_str());

            summary_table << summary;
        }

        summary_table.execute_group_by();
        return summary_table;
    }
};

} // namespace arena_benchmark
