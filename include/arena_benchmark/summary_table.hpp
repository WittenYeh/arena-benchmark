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
#include <vector>
#include <algorithm>
#include <iostream>
#include <arena_benchmark/benchmark_results.hpp>
#include <arena_benchmark/summary_table_visualizer.hpp>

namespace arena_benchmark {

// SummaryTable: Manages benchmark result data with grouping and sorting capabilities
class SummaryTable {

public:
    SummaryTable() : _group_by_key(""), _sort_by_key("") {}

    // Set grouping column by key
    auto set_group_by_key(const std::string& key) -> SummaryTable& {
        _group_by_key = key;
        return *this;
    }

    // Set sorting column by key (for sorting within groups)
    auto set_sort_by_key(const std::string& key) -> SummaryTable& {
        _sort_by_key = key;
        return *this;
    }

    // Add a result row to the table
    auto add_row(const MultiRepetitionSummary& result) -> void {
        _results.push_back(result);
    }

    // Execute grouping and sorting operations
    auto execute_group_by() -> void {
        if (_group_by_key.empty()) {
            // No grouping, just sort if sort_by is specified
            if (!_sort_by_key.empty()) {
                std::sort(_results.begin(), _results.end(), [this](const MultiRepetitionSummary& a, const MultiRepetitionSummary& b) {
                    return MultiRepetitionSummary::compare_by_key(a, b, _sort_by_key);
                });
            }
            return;
        }

        // Sort by group key first, then by sort key within groups
        std::sort(_results.begin(), _results.end(), [this](const MultiRepetitionSummary& a, const MultiRepetitionSummary& b) {
            std::string group_a = a.get_string_value(_group_by_key);
            std::string group_b = b.get_string_value(_group_by_key);

            if (group_a != group_b) {
                return group_a < group_b;
            }

            // Within same group, sort by sort_by_key if specified
            if (!_sort_by_key.empty()) {
                return MultiRepetitionSummary::compare_by_key(a, b, _sort_by_key);
            }

            return false;
        });
    }

    // Get the processed results
    auto get_results() const -> const std::vector<MultiRepetitionSummary>& {
        return _results;
    }

    // Get the group by key
    auto get_group_by_key() const -> const std::string& {
        return _group_by_key;
    }

    // Find indices of best and second-best by key.
    // ascending=true: lower is better (e.g. real_time); ascending=false: higher is better (e.g. items/sec)
    auto find_best_two(const std::string& key, bool ascending = true) const -> std::pair<size_t, size_t> {
        size_t best = SIZE_MAX, second = SIZE_MAX;
        auto better = [&](size_t a, size_t b) {
            return ascending ? MultiRepetitionSummary::compare_by_key(_results[a], _results[b], key)
                             : MultiRepetitionSummary::compare_by_key(_results[b], _results[a], key);
        };
        for (size_t i = 0; i < _results.size(); ++i) {
            if (!_results[i].has(key)) continue;
            if (best == SIZE_MAX || better(i, best)) { second = best; best = i; }
            else if (second == SIZE_MAX || better(i, second)) second = i;
        }
        return {best, second};
    }

    // Stream operator to add a row
    friend auto operator<<(SummaryTable& table, const MultiRepetitionSummary& result) -> SummaryTable& {
        table.add_row(result);
        return table;
    }

    // Friend function for ostream << operator
    friend std::ostream& operator<<(std::ostream& os, const SummaryTable& table) {
        SummaryTableVisualizer::visualize(table, os);
        return os;
    }

private:
    std::string _group_by_key;                   // Grouping key
    std::string _sort_by_key;                    // Sorting key (within groups)
    std::vector<MultiRepetitionSummary> _results; // Store results for sorting
};

} // namespace arena_benchmark
