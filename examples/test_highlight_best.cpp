#include <arena_benchmark/arena_benchmark.hpp>
#include <benchmark/benchmark.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <iostream>

using namespace arena_benchmark;

static auto make_data(size_t n) -> std::vector<int> {
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 0);
    std::shuffle(v.begin(), v.end(), std::mt19937{42});
    return v;
}

int main(int argc, char** argv) {
    auto data = make_data(10'000);

    // Test 1: With highlight enabled (default)
    std::cout << "\n=== Test 1: Highlight Best Enabled (Default) ===" << std::endl;
    {
        ArenaBenchmark bench;
        bench.register_benchmark("BM_Sort_Fast",
            [&data](benchmark::State& state) {
                for (auto _ : state) {
                    auto copy = data;
                    std::sort(copy.begin(), copy.end());
                    benchmark::DoNotOptimize(copy);
                }
            })
            .repetitions(5)
            .workload_scale(10'000)
            .extra_info("Fast sort");

        bench.register_benchmark("BM_Sort_Slow",
            [&data](benchmark::State& state) {
                for (auto _ : state) {
                    auto copy = data;
                    std::stable_sort(copy.begin(), copy.end());
                    benchmark::DoNotOptimize(copy);
                }
            })
            .repetitions(5)
            .workload_scale(10'000)
            .extra_info("Slow sort");

        bench.run_all(argc, argv);
    }

    // Test 2: With highlight disabled
    std::cout << "\n=== Test 2: Highlight Best Disabled ===" << std::endl;
    {
        ArenaBenchmark bench;
        bench.register_benchmark("BM_Sort_Fast",
            [&data](benchmark::State& state) {
                for (auto _ : state) {
                    auto copy = data;
                    std::sort(copy.begin(), copy.end());
                    benchmark::DoNotOptimize(copy);
                }
            })
            .repetitions(5)
            .workload_scale(10'000)
            .extra_info("Fast sort");

        bench.register_benchmark("BM_Sort_Slow",
            [&data](benchmark::State& state) {
                for (auto _ : state) {
                    auto copy = data;
                    std::stable_sort(copy.begin(), copy.end());
                    benchmark::DoNotOptimize(copy);
                }
            })
            .repetitions(5)
            .workload_scale(10'000)
            .extra_info("Slow sort");

        bench.highlight_best_enabled(false)
             .run_all(argc, argv);
    }

    return 0;
}
