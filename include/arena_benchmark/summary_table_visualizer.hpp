#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <tabulate/table.hpp>
#include <tabulate/termcolor.hpp>
#include <arena_benchmark/benchmark_results.hpp>
#include <arena_benchmark/table_meta.hpp>

namespace arena_benchmark {

class SummaryTableVisualizer {

    using ui_table_t = tabulate::Table;
    using ui_row_t = tabulate::Table::Row_t;

public:
    static auto set_benchmark_name_width(size_t width) -> void {
        _meta.instance_name_col(ColumnSetting("Benchmark", width));
    }

    static auto set_extra_info_width(size_t width) -> void {
        _meta.extra_info_col(ColumnSetting("Extra Info", width));
    }

    static auto highlight_best_enabled(bool enabled) -> void {
        _highlight_best_enabled = enabled;
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

    template<typename SummaryTableType>
    static void visualize(const SummaryTableType& summary_table, std::ostream& os = std::cout) {
        auto data_table = std::make_unique<ui_table_t>();
        auto wrapper_table = std::make_unique<ui_table_t>();

        _init_table_format(*data_table, *wrapper_table);

        const auto& results = summary_table.get_results();
        const auto& group_by_key = summary_table.get_group_by_key();
        _fill_data_table(*data_table, results, group_by_key);

        if (_highlight_best_enabled) {
            auto [rt_best, rt_second] = summary_table.find_best_two("avg_real_time", true);
            auto [ips_best, ips_second] = summary_table.find_best_two("avg_items_per_second", false);
            _highlight_best_two(*data_table, rt_best, rt_second, "avg_real_time");
            _highlight_best_two(*data_table, ips_best, ips_second, "avg_items_per_second");
        }

        std::stringstream colored_stream;
        termcolor::colorize(colored_stream);
        data_table->print(colored_stream);

        std::string rendered = colored_stream.str();
        size_t w = _rendered_line_width(rendered);

        termcolor::colorize(os);
        wrapper_table->add_row(ui_row_t{rendered});
        wrapper_table->row(1).format().hide_border_top().width(w + 2);

        os << *wrapper_table << std::endl;
    }

private:
    static SummaryTableMeta _meta;
    static bool _highlight_best_enabled;

    static void _init_table_format(ui_table_t& data_table, ui_table_t& wrapper_table) {
        wrapper_table.format()
            .multi_byte_characters(true)
            .border_top("═").border_bottom("═")
            .border_left("║").border_right("║")
            .corner_top_left("╔").corner_top_right("╗")
            .corner_bottom_left("╚").corner_bottom_right("╝")
            .column_separator("║");

        wrapper_table.add_row(ui_row_t{"Benchmark Summary Board"});
        wrapper_table.row(0).format()
            .font_align(tabulate::FontAlign::center)
            .font_style({tabulate::FontStyle::bold})
            .font_color(tabulate::Color::yellow);

        data_table.format()
            .multi_byte_characters(true)
            .border_top("─").border_bottom("─")
            .border_left("│").border_right("│")
            .corner_top_left("·").corner_top_right("·")
            .corner_bottom_left("·").corner_bottom_right("·")
            .column_separator("│");

        ui_row_t header_row;
        header_row.push_back(_meta.instance_name_col().column_name());
        header_row.push_back(_meta.repetitions_col().column_name());
        header_row.push_back(_meta.avg_real_time_col().column_name());
        header_row.push_back(_meta.median_real_time_col().column_name());
        header_row.push_back(_meta.avg_cpu_time_col().column_name());
        header_row.push_back(_meta.avg_items_per_second_col().column_name());
        header_row.push_back(_meta.extra_info_col().column_name());
        data_table.add_row(header_row);

        data_table.column(0).format().width(_meta.instance_name_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        data_table.column(1).format().width(_meta.repetitions_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        data_table.column(2).format().width(_meta.avg_real_time_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        data_table.column(3).format().width(_meta.median_real_time_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        data_table.column(4).format().width(_meta.avg_cpu_time_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        data_table.column(5).format().width(_meta.avg_items_per_second_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
        data_table.column(6).format().width(_meta.extra_info_col().column_width()).font_align(tabulate::FontAlign::center).font_style({tabulate::FontStyle::bold});
    }

    // Returns the display width of the first line of a string, ignoring ANSI codes and counting multi-byte UTF-8 as 1
    static size_t _rendered_line_width(const std::string& s) {
        size_t w = 0;
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\n') break;
            if (s[i] == '\x1b') { while (i < s.size() && s[i] != 'm') ++i; continue; }
            if ((s[i] & 0x80) == 0)        ++w;
            else if ((s[i] & 0xE0) == 0xC0) { ++w; i += 1; }
            else if ((s[i] & 0xF0) == 0xE0) { ++w; i += 2; }
            else if ((s[i] & 0xF8) == 0xF0) { ++w; i += 3; }
        }
        return w;
    }

    static void _highlight_best_two(ui_table_t& data_table, size_t best, size_t second, const std::string& key) {
        size_t col = _meta.col_index(key);
        if (col == SIZE_MAX) return;
        if (best != SIZE_MAX)
            data_table[best + 1][col].format()
                .font_background_color(tabulate::Color::red)
                .font_style({tabulate::FontStyle::bold, tabulate::FontStyle::underline});
        if (second != SIZE_MAX)
            data_table[second + 1][col].format()
                .font_background_color(tabulate::Color::yellow)
                .font_style({tabulate::FontStyle::bold, tabulate::FontStyle::underline});
    }

    static void _fill_data_table(ui_table_t& data_table, const std::vector<MultiRepetitionSummary>& results, const std::string& group_by_key) {
        std::string last_group_key;
        std::vector<size_t> hidden_border_rows;

        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            std::string current_group_key = group_by_key.empty() ? "" : result.get_string_value(group_by_key);

            ui_row_t row;
            row.push_back(result.instance_name());
            row.push_back(std::to_string(result.repetitions()));

            std::ostringstream avg_time_stream;
            avg_time_stream << std::fixed << std::setprecision(2) << result.avg_real_time() << " " << result.time_unit();
            row.push_back(avg_time_stream.str());

            std::ostringstream median_time_stream;
            median_time_stream << std::fixed << std::setprecision(2) << result.median_real_time() << " " << result.time_unit();
            row.push_back(median_time_stream.str());

            std::ostringstream cpu_time_stream;
            cpu_time_stream << std::fixed << std::setprecision(2) << result.avg_cpu_time() << " " << result.time_unit();
            row.push_back(cpu_time_stream.str());

            std::ostringstream items_stream;
            if (result.avg_items_per_second() == 0.0)
                items_stream << "---";
            else
                items_stream << std::fixed << std::setprecision(2) << result.avg_items_per_second();
            row.push_back(items_stream.str());

            row.push_back(result.extra_info().empty() ? "---" : result.extra_info());

            data_table.add_row(row);
            size_t current_row = data_table.size() - 1;

            if (i != 0 && (group_by_key.empty() || current_group_key == last_group_key)) {
                hidden_border_rows.push_back(current_row);
            }

            // Apply color coding: green for benchmark names, yellow for Reps, blue for time/data
            data_table[current_row][0].format().font_color(tabulate::Color::green);
            data_table[current_row][1].format().font_color(tabulate::Color::yellow);
            data_table[current_row][2].format().font_color(tabulate::Color::blue);
            data_table[current_row][3].format().font_color(tabulate::Color::blue);
            data_table[current_row][4].format().font_color(tabulate::Color::blue);
            data_table[current_row][5].format().font_color(tabulate::Color::blue);

            data_table.column(0).format().width(_meta.instance_name_col().column_width()).font_align(tabulate::FontAlign::center);
            data_table.column(1).format().width(_meta.repetitions_col().column_width()).font_align(tabulate::FontAlign::center);
            data_table.column(2).format().width(_meta.avg_real_time_col().column_width()).font_align(tabulate::FontAlign::center);
            data_table.column(3).format().width(_meta.median_real_time_col().column_width()).font_align(tabulate::FontAlign::center);
            data_table.column(4).format().width(_meta.avg_cpu_time_col().column_width()).font_align(tabulate::FontAlign::center);
            data_table.column(5).format().width(_meta.avg_items_per_second_col().column_width()).font_align(tabulate::FontAlign::center);
            data_table.column(6).format().width(_meta.extra_info_col().column_width()).font_align(tabulate::FontAlign::center);

            for (size_t r : hidden_border_rows) {
                data_table[r].format().hide_border_top();
            }

            last_group_key = current_group_key;
        }
    }
};

inline SummaryTableMeta SummaryTableVisualizer::_meta = SummaryTableMeta::default_meta();
inline bool SummaryTableVisualizer::_highlight_best_enabled = true;

} // namespace arena_benchmark
