#include <thread>
#include <chrono>
#include <arena_benchmark/repetition_log_list.hpp>
#include <arena_benchmark/benchmark_results.hpp>

int main() {
    using namespace arena_benchmark;

    RepetitionLogList log;

    SingleRepetitionResult result1;
    result1.instance_name("BM_Sort_Random_1K")
           .repetition_index(0)
           .iterations(100)
           .real_time(125.50)
           .cpu_time(124.30)
           .avg_time_per_item(125.50)
           .time_unit("us")
           .items_per_second(8032)
           .extra_info("std::sort")
           .error_occurred(false)
           .error_message("");

    SingleRepetitionResult result2;
    result2.instance_name("BM_Sort_Random_1K")
           .repetition_index(1)
           .iterations(100)
           .real_time(123.10)
           .cpu_time(122.80)
           .avg_time_per_item(123.10)
           .time_unit("us")
           .items_per_second(8120)
           .extra_info("std::sort")
           .error_occurred(false)
           .error_message("");

    SingleRepetitionResult result3;
    result3.instance_name("BM_Sort_Random_1K")
           .repetition_index(2)
           .iterations(100)
           .real_time(126.75)
           .cpu_time(125.90)
           .avg_time_per_item(126.75)
           .time_unit("us")
           .items_per_second(7890)
           .extra_info("std::sort")
           .error_occurred(false)
           .error_message("");

    log << result1;
    log << result2;
    log << result3;

    return 0;
}
