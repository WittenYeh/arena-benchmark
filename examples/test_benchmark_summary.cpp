#include <arena_benchmark/summary_table.hpp>
#include <arena_benchmark/benchmark_results.hpp>

int main() {
    using namespace arena_benchmark;

    SummaryTable summary_table;
    summary_table.set_sort_by_key("avg_real_time");

    MultiRepetitionSummary summary1;
    summary1.instance_name("BM_Sort_Random_1K")
            .repetitions(3)
            .avg_real_time(125.50)
            .avg_cpu_time(124.30)
            .avg_time_per_item(125.50)
            .avg_items_per_second(8032.12)
            .time_unit("us")
            .extra_info("std::sort");

    MultiRepetitionSummary summary2;
    summary2.instance_name("BM_Sort_Random_10K")
            .repetitions(3)
            .avg_real_time(1523.75)
            .avg_cpu_time(1520.45)
            .avg_time_per_item(1523.75)
            .avg_items_per_second(6567.89)
            .time_unit("us")
            .extra_info("std::sort");

    MultiRepetitionSummary summary3;
    summary3.instance_name("BM_Reverse_Sequential_1K")
            .repetitions(3)
            .avg_real_time(15.25)
            .avg_cpu_time(15.10)
            .avg_time_per_item(15.25)
            .avg_items_per_second(66225.16)
            .time_unit("us")
            .extra_info("std::reverse");

    MultiRepetitionSummary summary4;
    summary4.instance_name("BM_Search_Sequential_10K")
            .repetitions(3)
            .avg_real_time(8.75)
            .avg_cpu_time(8.60)
            .avg_time_per_item(8.75)
            .avg_items_per_second(1162790.70)
            .time_unit("us")
            .extra_info("binary_search");

    summary_table.add_row(summary1);
    summary_table.add_row(summary2);
    summary_table.add_row(summary3);
    summary_table.add_row(summary4);

    summary_table.execute_group_by();
    std::cout << summary_table;

    return 0;
}
