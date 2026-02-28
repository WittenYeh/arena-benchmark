#pragma once

#include <benchmark/benchmark.h>
#include <cassert>
#include <string>
#include <vector>
#include <arena_benchmark/benchmark_results.hpp>

namespace arena_benchmark {

/**
 * Custom reporter for Google Benchmark that stores individual benchmark results.
 *
 * This reporter is designed to work with benchmarks that have repetitions set to 1.
 * It captures each benchmark run and stores it as a SingleRepetitionResult, which
 * can be retrieved later for further processing or analysis.
 *
 * Requirements:
 * - Benchmarks must be configured with Repetitions(1)
 * - The reporter will assert if repetitions != 1
 *
 * Usage:
 *   ArenaReporter reporter;
 *   benchmark::RunSpecifiedBenchmarks(&reporter);
 *   auto results = reporter.get_results();
 */
class ArenaReporter : public benchmark::BenchmarkReporter {
public:
    ArenaReporter() = default;

    const std::vector<SingleRepetitionResult>& get_results() const {
        return _results;
    }

    const SystemContext& get_system_context() const {
        return _system_context;
    }

    void clear_results() {
        _results.clear();
    }

private:
    bool ReportContext(const Context& context) override {
        _system_context
            .num_cpus(context.cpu_info.num_cpus)
            .cpu_frequency_mhz(context.cpu_info.cycles_per_second / 1e6)
            .cpu_scaling_enabled(context.cpu_info.scaling == benchmark::CPUInfo::Scaling::ENABLED);
        return true;
    }

    void ReportRuns(const std::vector<Run>& reports) override {
        for (const auto& run : reports) {
            if (run.run_type == Run::RunType::RT_Aggregate)
                continue;

            SingleRepetitionResult result;
            result.instance_name(run.benchmark_name())
                  .repetition_index(static_cast<int>(run.repetition_index))
                  .iterations(static_cast<int64_t>(run.iterations))
                  .real_time(run.GetAdjustedRealTime())
                  .cpu_time(run.GetAdjustedCPUTime())
                  .time_unit(time_unit_to_string(run.time_unit))
                  .items_per_second(run.counters.count("items_per_second")
                      ? static_cast<int64_t>(run.counters.at("items_per_second")) : 0)
                  .extra_info("")
                  .error_occurred(run.skipped == benchmark::internal::SkippedWithError)
                  .error_message(run.skip_message);

            _results.push_back(result);
        }
    }

    void Finalize() override {}

    std::string time_unit_to_string(benchmark::TimeUnit unit) const {
        switch (unit) {
            case benchmark::TimeUnit::kNanosecond:  return "ns";
            case benchmark::TimeUnit::kMicrosecond: return "us";
            case benchmark::TimeUnit::kMillisecond: return "ms";
            case benchmark::TimeUnit::kSecond:      return "s";
            default:                                return "unknown";
        }
    }

    std::vector<SingleRepetitionResult> _results;
    SystemContext _system_context;
};

} // namespace arena_benchmark

