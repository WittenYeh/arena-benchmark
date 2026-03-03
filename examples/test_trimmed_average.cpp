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

    // Test 1: With sufficient repetitions (should use trimmed average)
    std::cout << "\n=== Test 1: Trimmed Average with 10 repetitions ===" << std::endl;
    {
        ArenaBenchmark bench;
        bench.register_benchmark("BM_Sort_10Reps",
            [&data](benchmark::State& state) {
                for (auto _ : state) {
                    auto copy = data;
                    std::sort(copy.begin(), copy.end());
                    benchmark::DoNotOptimize(copy);
                }
            })
            .repetitions(10)
            .workload_scale(10'000)
            .extra_info("10 reps with trimmed avg");

        bench.trimmed_avg_enabled(true)
             .run_all(argc, argv);
    }

    // Test 2: With insufficient repetitions (should show warning)
    std::cout << "\n=== Test 2: Trimmed Average with 3 repetitions (should warn) ===" << std::endl;
    {
        ArenaBenchmark bench;
        bench.register_benchmark("BM_Sort_3Reps",
            [&data](benchmark::State& state) {
                for (auto _ : state) {
                    auto copy = data;
                    std::sort(copy.begin(), copy.end());
                    benchmark::DoNotOptimize(copy);
                }
            })
            .repetitions(3)
            .workload_scale(10'000)
            .extra_info("3 reps with trimmed avg (warning expected)");

        bench.trimmed_avg_enabled(true)
             .run_all(argc, argv);
    }

    // Test 3: Without trimmed average (regular average)
    std::cout << "\n=== Test 3: Regular Average with 10 repetitions ===" << std::endl;
    {
        ArenaBenchmark bench;
        bench.register_benchmark("BM_Sort_RegularAvg",
            [&data](benchmark::State& state) {
                for (auto _ : state) {
                    auto copy = data;
                    std::sort(copy.begin(), copy.end());
                    benchmark::DoNotOptimize(copy);
                }
            })
            .repetitions(10)
            .workload_scale(10'000)
            .extra_info("10 reps with regular avg");

        bench.trimmed_avg_enabled(false)  // Explicitly disable
             .run_all(argc, argv);
    }

    return 0;
}
