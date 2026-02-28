#pragma once

#include <string>
#include <functional>
#include <stdexcept>
#include <benchmark/benchmark.h>

namespace arena_benchmark {

// Fluent builder returned by ArenaBenchmark::register_benchmark().
// Owns the benchmark configuration and is stored by ArenaBenchmark.
class InstanceRegistration {
public:
    InstanceRegistration(std::string name,
                         std::function<void(benchmark::State&)> func)
        : _name(std::move(name)), _func(std::move(func)) {}

    // Set number of repetitions for averaging
    auto repetitions(int n) -> InstanceRegistration& {
        _num_repetitions = n;
        return *this;
    }

    // Set extra info string shown in results
    auto extra_info(std::string info) -> InstanceRegistration& {
        _extra_info_str = std::move(info);
        return *this;
    }

    // Set workload scale for derived metrics:
    // - items/sec = workload_scale / real_time_per_iteration
    // - avg time/item = real_time_per_iteration / workload_scale
    // workload_scale must be >= 1.
    auto workload_scale(size_t scale) -> InstanceRegistration& {
        if (scale < 1) {
            throw std::invalid_argument("workload_scale must be >= 1");
        }
        _workload_scale = scale;
        return *this;
    }

    // Set time unit for this benchmark (e.g. benchmark::kMillisecond)
    auto time_unit(benchmark::TimeUnit unit) -> InstanceRegistration& {
        _time_unit = unit;
        return *this;
    }

    // Accessors used by ArenaBenchmark
    const std::string& name()           const { return _name; }
    const std::function<void(benchmark::State&)>& func() const { return _func; }
    int   num_repetitions()             const { return _num_repetitions; }
    const std::string& extra_info_str() const { return _extra_info_str; }
    size_t workload_scale()             const { return _workload_scale; }
    benchmark::TimeUnit time_unit()     const { return _time_unit; }

private:
    std::string _name;
    std::function<void(benchmark::State&)> _func;
    int _num_repetitions = 1;
    std::string _extra_info_str;
    size_t _workload_scale = 1;
    benchmark::TimeUnit _time_unit = benchmark::kMicrosecond;
};

} // namespace arena_benchmark
