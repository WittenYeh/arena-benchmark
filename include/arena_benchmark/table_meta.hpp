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
#include <cstddef>
#include <vector>

namespace arena_benchmark {

// Column setting for table display
struct ColumnSetting {
public:
    ColumnSetting() = default;
    ColumnSetting(std::string name, size_t width) : _column_name(std::move(name)), _column_width(width) {}
    ColumnSetting(const ColumnSetting&) = default;
    ColumnSetting(ColumnSetting&&) = default;
    ColumnSetting& operator=(const ColumnSetting&) = default;
    ColumnSetting& operator=(ColumnSetting&&) = default;

    auto column_name(std::string value) -> ColumnSetting& { _column_name = std::move(value); return *this; }
    auto column_width(size_t value) -> ColumnSetting& { _column_width = value; return *this; }

    auto column_name() const -> const std::string& { return _column_name; }
    auto column_width() const -> size_t { return _column_width; }

private:
    std::string _column_name;
    size_t _column_width = 0;
};

// Table meta for RepetitionLogList
struct RepetitionLogListMeta {
public:
    RepetitionLogListMeta() = default;
    RepetitionLogListMeta(const RepetitionLogListMeta&) = default;
    RepetitionLogListMeta(RepetitionLogListMeta&&) = default;
    RepetitionLogListMeta& operator=(const RepetitionLogListMeta&) = default;
    RepetitionLogListMeta& operator=(RepetitionLogListMeta&&) = default;

    auto instance_name_col(ColumnSetting value) -> RepetitionLogListMeta& { _cols["instance_name"] = std::move(value); return *this; }
    auto repetition_index_col(ColumnSetting value) -> RepetitionLogListMeta& { _cols["repetition_index"] = std::move(value); return *this; }
    auto iterations_col(ColumnSetting value) -> RepetitionLogListMeta& { _cols["iterations"] = std::move(value); return *this; }
    auto real_time_col(ColumnSetting value) -> RepetitionLogListMeta& { _cols["real_time"] = std::move(value); return *this; }
    auto cpu_time_col(ColumnSetting value) -> RepetitionLogListMeta& { _cols["cpu_time"] = std::move(value); return *this; }
    auto avg_time_per_item_col(ColumnSetting value) -> RepetitionLogListMeta& { _cols["avg_time_per_item"] = std::move(value); return *this; }
    auto items_per_second_col(ColumnSetting value) -> RepetitionLogListMeta& { _cols["items_per_second"] = std::move(value); return *this; }
    auto is_warm_up_col(ColumnSetting value) -> RepetitionLogListMeta& { _cols["is_warm_up"] = std::move(value); return *this; }
    auto extra_info_col(ColumnSetting value) -> RepetitionLogListMeta& { _cols["extra_info"] = std::move(value); return *this; }

    auto instance_name_col() const -> const ColumnSetting& { return _cols.at("instance_name"); }
    auto repetition_index_col() const -> const ColumnSetting& { return _cols.at("repetition_index"); }
    auto iterations_col() const -> const ColumnSetting& { return _cols.at("iterations"); }
    auto real_time_col() const -> const ColumnSetting& { return _cols.at("real_time"); }
    auto cpu_time_col() const -> const ColumnSetting& { return _cols.at("cpu_time"); }
    auto avg_time_per_item_col() const -> const ColumnSetting& { return _cols.at("avg_time_per_item"); }
    auto items_per_second_col() const -> const ColumnSetting& { return _cols.at("items_per_second"); }
    auto is_warm_up_col() const -> const ColumnSetting& { return _cols.at("is_warm_up"); }
    auto extra_info_col() const -> const ColumnSetting& { return _cols.at("extra_info"); }

    auto get(const std::string& key) const -> const ColumnSetting& { return _cols.at(key); }
    auto has(const std::string& key) const -> bool { return _cols.find(key) != _cols.end(); }
    auto cols() const -> const std::map<std::string, ColumnSetting>& { return _cols; }

    auto get_total_width() const -> size_t {
        size_t total = 0;
        for (const auto& [key, col] : _cols) total += col.column_width();
        return total;
    }

    auto get_rendered_inner_width() const -> size_t {
        if (_cols.empty()) return 0;
        return get_total_width() + (_cols.size() - 1);
    }

    static RepetitionLogListMeta default_meta() {
        RepetitionLogListMeta meta;
        meta.instance_name_col(ColumnSetting("Benchmark", 30))
            .repetition_index_col(ColumnSetting("Rep#", 6))
            .iterations_col(ColumnSetting("Iters", 8))
            .real_time_col(ColumnSetting("Real Time", 18))
            .cpu_time_col(ColumnSetting("CPU Time", 18))
            .avg_time_per_item_col(ColumnSetting("Avg Time/Item", 22))
            .items_per_second_col(ColumnSetting("Items/sec", 18))
            .is_warm_up_col(ColumnSetting("WarmUp", 8))
            .extra_info_col(ColumnSetting("Extra Info", 20));
        return meta;
    }

private:
    std::map<std::string, ColumnSetting> _cols;
};

// Table meta for SummaryTable
struct SummaryTableMeta {
public:
    SummaryTableMeta() = default;
    SummaryTableMeta(const SummaryTableMeta&) = default;
    SummaryTableMeta(SummaryTableMeta&&) = default;
    SummaryTableMeta& operator=(const SummaryTableMeta&) = default;
    SummaryTableMeta& operator=(SummaryTableMeta&&) = default;

    auto instance_name_col(ColumnSetting value) -> SummaryTableMeta& { _cols["instance_name"] = std::move(value); return *this; }
    auto repetitions_col(ColumnSetting value) -> SummaryTableMeta& { _cols["repetitions"] = std::move(value); return *this; }
    auto avg_real_time_col(ColumnSetting value) -> SummaryTableMeta& { _cols["avg_real_time"] = std::move(value); return *this; }
    auto median_real_time_col(ColumnSetting value) -> SummaryTableMeta& { _cols["median_real_time"] = std::move(value); return *this; }
    auto avg_cpu_time_col(ColumnSetting value) -> SummaryTableMeta& { _cols["avg_cpu_time"] = std::move(value); return *this; }
    auto avg_items_per_second_col(ColumnSetting value) -> SummaryTableMeta& { _cols["avg_items_per_second"] = std::move(value); return *this; }
    auto extra_info_col(ColumnSetting value) -> SummaryTableMeta& { _cols["extra_info"] = std::move(value); return *this; }

    auto instance_name_col() const -> const ColumnSetting& { return _cols.at("instance_name"); }
    auto repetitions_col() const -> const ColumnSetting& { return _cols.at("repetitions"); }
    auto avg_real_time_col() const -> const ColumnSetting& { return _cols.at("avg_real_time"); }
    auto median_real_time_col() const -> const ColumnSetting& { return _cols.at("median_real_time"); }
    auto avg_cpu_time_col() const -> const ColumnSetting& { return _cols.at("avg_cpu_time"); }
    auto avg_items_per_second_col() const -> const ColumnSetting& { return _cols.at("avg_items_per_second"); }
    auto extra_info_col() const -> const ColumnSetting& { return _cols.at("extra_info"); }

    auto get(const std::string& key) const -> const ColumnSetting& { return _cols.at(key); }
    auto has(const std::string& key) const -> bool { return _cols.find(key) != _cols.end(); }
    auto cols() const -> const std::map<std::string, ColumnSetting>& { return _cols; }

    // Returns the display column index for a given key
    auto col_index(const std::string& key) const -> size_t {
        static const std::vector<std::string> order = {
            "instance_name", "repetitions", "avg_real_time", "median_real_time",
            "avg_cpu_time", "avg_items_per_second", "extra_info"
        };
        for (size_t i = 0; i < order.size(); ++i)
            if (order[i] == key) return i;
        return SIZE_MAX;
    }

    auto get_total_width() const -> size_t {
        size_t total = 0;
        for (const auto& [key, col] : _cols) total += col.column_width();
        return total;
    }

    static SummaryTableMeta default_meta() {
        SummaryTableMeta meta;
        meta.instance_name_col(ColumnSetting("Benchmark", 30))
            .repetitions_col(ColumnSetting("Reps", 6))
            .avg_real_time_col(ColumnSetting("Avg Time", 18))
            .median_real_time_col(ColumnSetting("Median Time", 18))
            .avg_cpu_time_col(ColumnSetting("CPU Time", 18))
            .avg_items_per_second_col(ColumnSetting("Items/sec", 18))
            .extra_info_col(ColumnSetting("Extra Info", 20));
        return meta;
    }

private:
    std::map<std::string, ColumnSetting> _cols;
};

}  // namespace arena_benchmark
