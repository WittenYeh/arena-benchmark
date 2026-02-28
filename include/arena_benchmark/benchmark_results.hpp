// Copyright 2026 Weitang Ye
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <string>
#include <map>
#include <cstdint>
#include <variant>
#include <sstream>

namespace arena_benchmark {

// System context information captured during benchmark execution
// Supports Builder Pattern for flexible construction
struct SystemContext {
public:
    // Builder pattern setters (compact single-line form)
    auto num_cpus(int value) -> SystemContext& { _num_cpus = value; return *this; }
    auto cpu_frequency_mhz(double value) -> SystemContext& { _cpu_frequency_mhz = value; return *this; }
    auto cpu_scaling_enabled(bool value) -> SystemContext& { _cpu_scaling_enabled = value; return *this; }

    // Const getters
    auto num_cpus() const -> int { return _num_cpus; }
    auto cpu_frequency_mhz() const -> double { return _cpu_frequency_mhz; }
    auto cpu_scaling_enabled() const -> bool { return _cpu_scaling_enabled; }

private:
    int _num_cpus = 0;                    // Number of CPUs available
    double _cpu_frequency_mhz = 0.0;      // CPU frequency in MHz
    bool _cpu_scaling_enabled = false;    // Whether CPU scaling is enabled
};  // struct SystemContext

// Result from a single benchmark repetition
// This structure stores the complete result of one benchmark run
// Supports Builder Pattern for flexible construction
struct SingleRepetitionResult {
public:
    SingleRepetitionResult() {
        _data["is_warm_up"] = false;
        _data["avg_time_per_item"] = 0.0;
    }

    // Value type for flexible data storage
    using Value = std::variant<std::string, int, int64_t, double, bool, size_t>;

    // Builder pattern setters (compact single-line form)
    auto instance_name(std::string value) -> SingleRepetitionResult& { _data["instance_name"] = std::move(value); return *this; }
    auto repetition_index(int value) -> SingleRepetitionResult& { _data["repetition_index"] = value; return *this; }
    auto iterations(int64_t value) -> SingleRepetitionResult& { _data["iterations"] = value; return *this; }
    auto real_time(double value) -> SingleRepetitionResult& { _data["real_time"] = value; return *this; }
    auto cpu_time(double value) -> SingleRepetitionResult& { _data["cpu_time"] = value; return *this; }
    auto avg_time_per_item(double value) -> SingleRepetitionResult& { _data["avg_time_per_item"] = value; return *this; }
    auto time_unit(std::string value) -> SingleRepetitionResult& { _data["time_unit"] = std::move(value); return *this; }
    auto items_per_second(int64_t value) -> SingleRepetitionResult& { _data["items_per_second"] = value; return *this; }
    auto extra_info(std::string value) -> SingleRepetitionResult& { _data["extra_info"] = std::move(value); return *this; }
    auto is_warm_up(bool value) -> SingleRepetitionResult& { _data["is_warm_up"] = value; return *this; }
    auto error_occurred(bool value) -> SingleRepetitionResult& { _data["error_occurred"] = value; return *this; }
    auto error_message(std::string value) -> SingleRepetitionResult& { _data["error_message"] = std::move(value); return *this; }

    // Compute and set items_per_second from workload_scale and real_time/time_unit
    auto compute_items_per_second(size_t workload_scale) -> SingleRepetitionResult& {
        double secs = real_time() * _time_unit_to_seconds(time_unit());
        if (secs > 0)
            items_per_second(static_cast<int64_t>(workload_scale / secs));
        return *this;
    }

    // Compute and set avg_time_per_item from real_time/workload_scale.
    // The output keeps the same time_unit as real_time (e.g. us/item).
    auto compute_avg_time_per_item(size_t workload_scale) -> SingleRepetitionResult& {
        avg_time_per_item(real_time() / static_cast<double>(workload_scale));
        return *this;
    }

    // Const getters
    auto instance_name() const -> const std::string& { return std::get<std::string>(_data.at("instance_name")); }
    auto repetition_index() const -> int { return std::get<int>(_data.at("repetition_index")); }
    auto iterations() const -> int64_t { return std::get<int64_t>(_data.at("iterations")); }
    auto real_time() const -> double { return std::get<double>(_data.at("real_time")); }
    auto cpu_time() const -> double { return std::get<double>(_data.at("cpu_time")); }
    auto avg_time_per_item() const -> double { return std::get<double>(_data.at("avg_time_per_item")); }
    auto time_unit() const -> const std::string& { return std::get<std::string>(_data.at("time_unit")); }
    auto items_per_second() const -> int64_t { return std::get<int64_t>(_data.at("items_per_second")); }
    auto extra_info() const -> const std::string& { return std::get<std::string>(_data.at("extra_info")); }
    auto is_warm_up() const -> bool { return std::get<bool>(_data.at("is_warm_up")); }
    auto error_occurred() const -> bool { return std::get<bool>(_data.at("error_occurred")); }
    auto error_message() const -> const std::string& { return std::get<std::string>(_data.at("error_message")); }

    // Generic getter by key
    auto get(const std::string& key) const -> const Value& { return _data.at(key); }

    // Check if key exists
    auto has(const std::string& key) const -> bool { return _data.find(key) != _data.end(); }

    // Get all data
    auto data() const -> const std::map<std::string, Value>& { return _data; }

    // Get string representation of a value by key
    auto get_string_value(const std::string& key) const -> std::string {
        if (!has(key)) return "";
        const auto& value = get(key);
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::to_string(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<size_t>(value)) {
            return std::to_string(std::get<size_t>(value));
        }
        return "";
    }

private:
    std::map<std::string, Value> _data;  // Flexible data storage

    static auto _time_unit_to_seconds(const std::string& unit) -> double {
        if (unit == "ns") return 1e-9;
        if (unit == "us") return 1e-6;
        if (unit == "ms") return 1e-3;
        return 1.0;
    }
};

// Summary statistics across multiple benchmark repetitions
// Supports Builder Pattern for flexible construction
struct MultiRepetitionSummary {
public:
    MultiRepetitionSummary() {
        _data["avg_time_per_item"] = 0.0;
    }

    // Value type for flexible data storage
    using Value = std::variant<std::string, int, double, size_t>;

    // Builder pattern setters (compact single-line form)
    auto instance_name(std::string value) -> MultiRepetitionSummary& { _data["instance_name"] = std::move(value); return *this; }
    auto repetitions(int value) -> MultiRepetitionSummary& { _data["repetitions"] = value; return *this; }
    auto avg_real_time(double value) -> MultiRepetitionSummary& { _data["avg_real_time"] = value; return *this; }
    auto avg_cpu_time(double value) -> MultiRepetitionSummary& { _data["avg_cpu_time"] = value; return *this; }
    auto avg_time_per_item(double value) -> MultiRepetitionSummary& { _data["avg_time_per_item"] = value; return *this; }
    auto avg_items_per_second(double value) -> MultiRepetitionSummary& { _data["avg_items_per_second"] = value; return *this; }
    auto time_unit(std::string value) -> MultiRepetitionSummary& { _data["time_unit"] = std::move(value); return *this; }
    auto extra_info(std::string value) -> MultiRepetitionSummary& { _data["extra_info"] = std::move(value); return *this; }


    // Const getters
    auto instance_name() const -> const std::string& { return std::get<std::string>(_data.at("instance_name")); }
    auto repetitions() const -> int { return std::get<int>(_data.at("repetitions")); }
    auto avg_real_time() const -> double { return std::get<double>(_data.at("avg_real_time")); }
    auto avg_cpu_time() const -> double { return std::get<double>(_data.at("avg_cpu_time")); }
    auto avg_time_per_item() const -> double { return std::get<double>(_data.at("avg_time_per_item")); }
    auto avg_items_per_second() const -> double { return std::get<double>(_data.at("avg_items_per_second")); }
    auto time_unit() const -> const std::string& { return std::get<std::string>(_data.at("time_unit")); }
    auto extra_info() const -> const std::string& { return std::get<std::string>(_data.at("extra_info")); }


    // Generic getter by key
    auto get(const std::string& key) const -> const Value& { return _data.at(key); }

    // Check if key exists
    auto has(const std::string& key) const -> bool { return _data.find(key) != _data.end(); }

    // Get all data
    auto data() const -> const std::map<std::string, Value>& { return _data; }

    // Get string representation of a value by key
    auto get_string_value(const std::string& key) const -> std::string {
        if (!has(key)) {
            return "";
        }

        const auto& value = get(key);

        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<size_t>(value)) {
            return std::to_string(std::get<size_t>(value));
        }

        return "";
    }

    // Static comparison function: compare two results by a specific key
    static auto compare_by_key(const MultiRepetitionSummary& a, const MultiRepetitionSummary& b, const std::string& key) -> bool {
        if (!a.has(key) || !b.has(key)) {
            return false;
        }

        const auto& val_a = a.get(key);
        const auto& val_b = b.get(key);

        // Compare based on type
        if (std::holds_alternative<std::string>(val_a) && std::holds_alternative<std::string>(val_b)) {
            return std::get<std::string>(val_a) < std::get<std::string>(val_b);
        } else if (std::holds_alternative<int>(val_a) && std::holds_alternative<int>(val_b)) {
            return std::get<int>(val_a) < std::get<int>(val_b);
        } else if (std::holds_alternative<double>(val_a) && std::holds_alternative<double>(val_b)) {
            return std::get<double>(val_a) < std::get<double>(val_b);
        }

        return false;
    }

private:
    std::map<std::string, Value> _data;  // Flexible data storage
};

}  // namespace arena_benchmark
