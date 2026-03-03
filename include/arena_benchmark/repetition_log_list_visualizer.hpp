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

#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <string>
#include <tabulate/table.hpp>
#include <arena_benchmark/benchmark_results.hpp>
#include <arena_benchmark/table_meta.hpp>

namespace arena_benchmark {

class RepetitionLogListVisualizer {

    using ui_table_t = tabulate::Table;
    using ui_row_t = tabulate::Table::Row_t;

public:
    static auto or_default(const std::string& s) -> const std::string& {
        static const std::string placeholder = "---";
        return s.empty() ? placeholder : s;
    }

    static auto init_title_format(ui_table_t& title_table) -> void {
        const std::string title = "Benchmark Repetition Log Board";
        title_table.format()
            .multi_byte_characters(true)
            .border_top("─").border_bottom("─")
            .border_left("│").border_right("│")
            .corner_top_left("·").corner_top_right("·")
            .corner_bottom_left("·").corner_bottom_right("·")
            .column_separator("│");
        title_table.add_row(ui_row_t{title});
        title_table.column(0).format()
            .width(_table_meta.get_rendered_inner_width())
            .font_align(tabulate::FontAlign::center)
            .font_style({tabulate::FontStyle::bold})
            .font_color(tabulate::Color::yellow);
    }

    static auto init_header_format(ui_table_t& header_table) -> void {
        header_table.format()
            .multi_byte_characters(true)
            .border_top("─").border_bottom("─")
            .border_left("│").border_right("│")
            .corner_top_left("·").corner_top_right("·")
            .corner_bottom_left("·").corner_bottom_right("·")
            .column_separator("│");

        ui_row_t header_row;
        header_row.push_back(_table_meta.instance_name_col().column_name());
        header_row.push_back(_table_meta.repetition_index_col().column_name());
        header_row.push_back(_table_meta.iterations_col().column_name());
        header_row.push_back(_table_meta.real_time_col().column_name());
        header_row.push_back(_table_meta.cpu_time_col().column_name());
        header_row.push_back(_table_meta.items_per_second_col().column_name());
        header_row.push_back(_table_meta.is_warm_up_col().column_name());
        header_row.push_back(_table_meta.extra_info_col().column_name());
        header_table.add_row(header_row);
        header_table.row(0).format().hide_border_top().font_style({tabulate::FontStyle::bold});

        header_table.column(0).format().width(_table_meta.instance_name_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        header_table.column(1).format().width(_table_meta.repetition_index_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        header_table.column(2).format().width(_table_meta.iterations_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        header_table.column(3).format().width(_table_meta.real_time_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        header_table.column(4).format().width(_table_meta.cpu_time_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        header_table.column(5).format().width(_table_meta.items_per_second_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        header_table.column(6).format().width(_table_meta.is_warm_up_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        header_table.column(7).format().width(_table_meta.extra_info_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
    }

    static auto init_log_format(ui_table_t& single_log_table, const SingleRepetitionResult& result) -> void {
        single_log_table.format()
            .multi_byte_characters(true)
            .border_top("─").border_bottom("─")
            .border_left("│").border_right("│")
            .corner_top_left("·").corner_top_right("·")
            .corner_bottom_left("·").corner_bottom_right("·")
            .column_separator("│");

        ui_row_t log_row;
        log_row.push_back(result.instance_name());
        log_row.push_back(result.is_warm_up() ? "-" : std::to_string(result.repetition_index()));
        log_row.push_back(std::to_string(result.iterations()));

        std::ostringstream real_time_ss;
        real_time_ss << std::fixed << std::setprecision(2) << result.real_time() << " " << result.time_unit();
        log_row.push_back(real_time_ss.str());

        std::ostringstream cpu_time_ss;
        cpu_time_ss << std::fixed << std::setprecision(2) << result.cpu_time() << " " << result.time_unit();
        log_row.push_back(cpu_time_ss.str());

        log_row.push_back(result.items_per_second() == 0 ? "---" : std::to_string(result.items_per_second()));
        log_row.push_back(result.is_warm_up() ? "YES" : "NO");
        log_row.push_back(result.extra_info().empty() ? "---" : result.extra_info());
        single_log_table.add_row(log_row);
        single_log_table.row(0).format().hide_border_top();

        // Apply color coding: green for benchmark names, yellow for Rep#, blue for time/data
        single_log_table[0][0].format().font_color(tabulate::Color::green);
        single_log_table[0][1].format().font_color(tabulate::Color::yellow);
        single_log_table[0][2].format().font_color(tabulate::Color::blue);
        single_log_table[0][3].format().font_color(tabulate::Color::blue);
        single_log_table[0][4].format().font_color(tabulate::Color::blue);
        single_log_table[0][5].format().font_color(tabulate::Color::blue);
        single_log_table[0][6].format().font_color(tabulate::Color::blue);

        single_log_table.column(0).format().width(_table_meta.instance_name_col().column_width()).font_align(tabulate::FontAlign::center);
        single_log_table.column(1).format().width(_table_meta.repetition_index_col().column_width()).font_align(tabulate::FontAlign::center);
        single_log_table.column(2).format().width(_table_meta.iterations_col().column_width()).font_align(tabulate::FontAlign::center);
        single_log_table.column(3).format().width(_table_meta.real_time_col().column_width()).font_align(tabulate::FontAlign::center);
        single_log_table.column(4).format().width(_table_meta.cpu_time_col().column_width()).font_align(tabulate::FontAlign::center);
        single_log_table.column(5).format().width(_table_meta.items_per_second_col().column_width()).font_align(tabulate::FontAlign::center);
        single_log_table.column(6).format().width(_table_meta.is_warm_up_col().column_width()).font_align(tabulate::FontAlign::center);
        single_log_table.column(7).format().width(_table_meta.extra_info_col().column_width()).font_align(tabulate::FontAlign::center);
    }

    static auto display_title_and_header(std::ostream& os = std::cout) -> void {
        auto title_table = std::make_unique<ui_table_t>();
        auto header_table = std::make_unique<ui_table_t>();
        init_title_format(*title_table);
        init_header_format(*header_table);
        os << *title_table << std::endl;
        os << *header_table << std::endl;
    }

    static auto display_log(const SingleRepetitionResult& result, std::ostream& os = std::cout) -> void {
        auto single_log_table = std::make_unique<ui_table_t>();
        init_log_format(*single_log_table, result);
        os << *single_log_table << std::endl;
    }

    static auto set_benchmark_name_width(size_t width) -> void {
        _table_meta.instance_name_col(ColumnSetting("Benchmark", width));
    }

    static auto set_extra_info_width(size_t width) -> void {
        _table_meta.extra_info_col(ColumnSetting("Extra Info", width));
    }

    template<typename InstanceRange>
    static auto auto_size_columns(const InstanceRange& instances) -> void {
        size_t max_name = 0, max_extra = 0;
        for (const auto& inst : instances) {
            max_name  = std::max(max_name,  inst.name().size());
            max_extra = std::max(max_extra, inst.extra_info_str().size());
        }
        // Consider the column header width as well
        constexpr size_t benchmark_header_len = 9;  // "Benchmark"
        constexpr size_t extra_info_header_len = 10; // "Extra Info"
        max_name = std::max(max_name, benchmark_header_len);
        max_extra = std::max(max_extra, extra_info_header_len);

        set_benchmark_name_width(max_name + 2);
        set_extra_info_width(max_extra + 2);
    }

private:
    static RepetitionLogListMeta _table_meta;
};

inline RepetitionLogListMeta RepetitionLogListVisualizer::_table_meta = RepetitionLogListMeta::default_meta();

}   // namespace arena_benchmark
