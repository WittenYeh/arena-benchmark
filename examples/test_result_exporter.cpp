#include <iostream>

#include <arena_benchmark/result_exporter.hpp>

int main() {
    using namespace arena_benchmark;

    RepetitionLogList log_board;

    SingleRepetitionResult rep0;
    rep0.instance_name("BM_Sort_Random_1K")
        .repetition_index(0)
        .iterations(100)
        .real_time(125.50)
        .cpu_time(124.30)
        .time_unit("us")
        .items_per_second(8032)
        .extra_info("std::sort")
        .error_occurred(false)
        .error_message("");

    SingleRepetitionResult rep1;
    rep1.instance_name("BM_Sort_Random_1K")
        .repetition_index(1)
        .iterations(100)
        .real_time(123.10)
        .cpu_time(122.80)
        .time_unit("us")
        .items_per_second(8120)
        .extra_info("std::sort")
        .error_occurred(false)
        .error_message("");

    log_board << rep0;
    log_board << rep1;

    SummaryTable summary_board;
    summary_board.set_sort_by_key("avg_real_time");

    MultiRepetitionSummary summary;
    summary.instance_name("BM_Sort_Random_1K")
        .repetitions(2)
        .avg_real_time((125.50 + 123.10) / 2.0)
        .avg_cpu_time((124.30 + 122.80) / 2.0)
        .avg_items_per_second((8032.0 + 8120.0) / 2.0)
        .time_unit("us")
        .extra_info("std::sort");

    summary_board << summary;
    summary_board.execute_group_by();

    ResultExporter exporter("benchmark_logs", "benchmark_results");
    const auto log_json_path = exporter.write_repetition_log_board(log_board);
    const auto summary_json_path = exporter.write_summary_board(summary_board);

    std::cout << "Wrote repetition logs JSON: " << log_json_path << '\n';
    std::cout << "Wrote summary board JSON: " << summary_json_path << '\n';
    return 0;
}
