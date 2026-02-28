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
#include <arena_benchmark/repetition_log_list_visualizer.hpp>

namespace arena_benchmark {

class RepetitionLogList {

public:
    RepetitionLogList() {
        RepetitionLogListVisualizer::display_title_and_header();
    }

    auto add_row(const SingleRepetitionResult& result) -> void {
        _results.push_back(result);
    }

    // Stream operator to add a row
    friend auto operator<<(RepetitionLogList& list, const SingleRepetitionResult& result) -> RepetitionLogList& {
        list.add_row(result);
        RepetitionLogListVisualizer::display_log(result);
        return list;
    }

    auto get_results() const -> const std::vector<SingleRepetitionResult>& {
        return _results;
    }

private:

    std::vector<SingleRepetitionResult> _results;

};  // class RepetitionLogList

};  //  namespace arena_benchmark