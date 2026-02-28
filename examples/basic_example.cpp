#include <arena_benchmark/arena_benchmark.hpp>
#include <benchmark/benchmark.h>
#include <filesystem>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>

using namespace arena_benchmark;

static auto make_data(size_t n) -> std::vector<int> {
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 0);
    std::shuffle(v.begin(), v.end(), std::mt19937{42});
    return v;
}

int main(int argc, char** argv) {
    ArenaBenchmark bench;
    const std::filesystem::path export_root = ".";
    const auto logs_dir = export_root / "benchmark_logs";
    const auto results_dir = export_root / "benchmark_results";

    auto small  = make_data(1'000);
    auto medium = make_data(10'000);
    auto large  = make_data(100'000);

    bench.register_benchmark("BM_StdSort_Small",
        [&small](benchmark::State& state) {
            for (auto _ : state) {
                auto copy = small;
                std::sort(copy.begin(), copy.end());
                benchmark::DoNotOptimize(copy);
            }
        })
        .repetitions(3)
        .workload_scale(1'000)
        .time_unit(benchmark::kMillisecond)
        .extra_info("std::sort / 1K");

    bench.register_benchmark("BM_StdSort_Medium",
        [&medium](benchmark::State& state) {
            for (auto _ : state) {
                auto copy = medium;
                std::sort(copy.begin(), copy.end());
                benchmark::DoNotOptimize(copy);
            }
        })
        .repetitions(3)
        .extra_info("std::sort / 10K");

    bench.register_benchmark("BM_StdSort_Large",
        [&large](benchmark::State& state) {
            for (auto _ : state) {
                auto copy = large;
                std::sort(copy.begin(), copy.end());
                benchmark::DoNotOptimize(copy);
            }
        })
        .repetitions(3)
        .extra_info("std::sort / 100K");

    bench.register_benchmark("BM_StableSort_Small",
        [&small](benchmark::State& state) {
            for (auto _ : state) {
                auto copy = small;
                std::stable_sort(copy.begin(), copy.end());
                benchmark::DoNotOptimize(copy);
            }
        })
        .repetitions(3)
        .extra_info("std::stable_sort / 1K");

    bench.register_benchmark("BM_StableSort_Large",
        [&large](benchmark::State& state) {
            for (auto _ : state) {
                auto copy = large;
                std::stable_sort(copy.begin(), copy.end());
                benchmark::DoNotOptimize(copy);
            }
        })
        .repetitions(3)
        .extra_info("std::stable_sort / 100K");

    // Chain call: warm up, run benchmarks, then export JSON results.
    bench.warm_up(1).run_all(argc, argv).export_results(export_root);

    std::cout << "\nExport completed.\n"
              << "Repetition logs dir: " << std::filesystem::absolute(logs_dir) << '\n'
              << "Summary results dir: " << std::filesystem::absolute(results_dir) << '\n';
    return 0;
}
