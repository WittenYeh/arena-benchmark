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

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

#include <arena_benchmark/repetition_log_list.hpp>
#include <arena_benchmark/summary_table.hpp>

namespace arena_benchmark {

// Dump benchmark boards to JSON files.
// - Repetition Log Board -> benchmark_logs directory
// - Summary Board -> benchmark_results directory
class ResultExporter {
public:
    explicit ResultExporter(std::filesystem::path benchmark_logs_dir = "benchmark_logs",
                                   std::filesystem::path benchmark_results_dir = "benchmark_results")
        : _benchmark_logs_dir(std::move(benchmark_logs_dir)),
          _benchmark_results_dir(std::move(benchmark_results_dir)) {}

    auto set_benchmark_logs_dir(std::filesystem::path dir) -> ResultExporter& {
        _benchmark_logs_dir = std::move(dir);
        return *this;
    }

    auto set_benchmark_results_dir(std::filesystem::path dir) -> ResultExporter& {
        _benchmark_results_dir = std::move(dir);
        return *this;
    }

    auto benchmark_logs_dir() const -> const std::filesystem::path& {
        return _benchmark_logs_dir;
    }

    auto benchmark_results_dir() const -> const std::filesystem::path& {
        return _benchmark_results_dir;
    }

    auto write_repetition_log_board(
        const RepetitionLogList& log_board,
        const std::string& file_name = "benchmark_repetition_log_board.json") const
        -> std::filesystem::path {

        nlohmann::json root;
        root["board_name"] = "Benchmark Repetition Log Board";
        root["generated_at_unix_ms"] = _current_unix_ms();

        const auto& rows = log_board.get_results();
        root["row_count"] = rows.size();
        root["rows"] = nlohmann::json::array();

        for (const auto& row : rows) {
            root["rows"].push_back(_data_map_to_json(row.data()));
        }

        const auto out_path = _resolve_output_path(
            _benchmark_logs_dir, file_name, "benchmark_repetition_log_board.json");
        _write_json(out_path, root);
        return out_path;
    }

    auto write_summary_board(
        const SummaryTable& summary_board,
        const std::string& file_name = "benchmark_summary_board.json") const
        -> std::filesystem::path {

        nlohmann::json root;
        root["board_name"] = "Benchmark Summary Board";
        root["generated_at_unix_ms"] = _current_unix_ms();
        root["group_by_key"] = summary_board.get_group_by_key();

        const auto& rows = summary_board.get_results();
        root["row_count"] = rows.size();
        root["rows"] = nlohmann::json::array();

        for (const auto& row : rows) {
            root["rows"].push_back(_data_map_to_json(row.data()));
        }

        const auto out_path = _resolve_output_path(
            _benchmark_results_dir, file_name, "benchmark_summary_board.json");
        _write_json(out_path, root);
        return out_path;
    }

private:
    std::filesystem::path _benchmark_logs_dir;
    std::filesystem::path _benchmark_results_dir;

    template <typename VariantType>
    static auto _variant_to_json(const VariantType& value) -> nlohmann::json {
        return std::visit([](const auto& v) -> nlohmann::json { return v; }, value);
    }

    template <typename MapType>
    static auto _data_map_to_json(const MapType& data_map) -> nlohmann::json {
        nlohmann::json out = nlohmann::json::object();
        for (const auto& [key, value] : data_map) {
            out[key] = _variant_to_json(value);
        }
        return out;
    }

    static auto _normalize_json_name(std::string file_name, const std::string& fallback_name) -> std::string {
        if (file_name.empty()) {
            file_name = fallback_name;
        }

        const std::filesystem::path input_path(file_name);
        if (input_path.is_absolute()) {
            throw std::invalid_argument("file_name must be a base name, not an absolute path");
        }
        if (input_path.has_parent_path()) {
            throw std::invalid_argument("file_name must not contain directory components");
        }

        std::string name = input_path.filename().string();
        if (name.empty() || name == "." || name == "..") {
            throw std::invalid_argument("file_name must be a valid file base name");
        }
        if (name.size() < 5 || name.substr(name.size() - 5) != ".json") {
            name += ".json";
        }
        return name;
    }

    static auto _resolve_output_path(const std::filesystem::path& out_dir,
                                     const std::string& file_name,
                                     const std::string& fallback_name) -> std::filesystem::path {
        if (out_dir.empty()) {
            throw std::invalid_argument("Output directory must not be empty");
        }
        return out_dir / _normalize_json_name(file_name, fallback_name);
    }

    static auto _write_json(const std::filesystem::path& out_path, const nlohmann::json& payload) -> void {
        if (!out_path.has_parent_path()) {
            throw std::runtime_error("Output path has no parent directory: " + out_path.string());
        }

        std::filesystem::create_directories(out_path.parent_path());

        std::ofstream ofs(out_path);
        if (!ofs.is_open()) {
            throw std::runtime_error("Failed to open output file: " + out_path.string());
        }

        ofs << payload.dump(2) << '\n';
    }

    static auto _current_unix_ms() -> std::int64_t {
        const auto now = std::chrono::system_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        return static_cast<std::int64_t>(ms.count());
    }
};

}  // namespace arena_benchmark
