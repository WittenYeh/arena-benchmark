```
    ___                        ____                  __                          __
   /   |  ________  ____  ____ / __ )___  ____  _____/ /_  ____ ___  ____ ______/ /__
  / /| | / ___/ _ \/ __ \/ __ `/ __  / _ \/ __ \/ ___/ __ \/ __ `__ \/ __ `/ ___/ //_/
 / ___ |/ /  /  __/ / / / /_/ / /_/ /  __/ / / / /__/ / / / / / / / / /_/ / /  / ,<
/_/  |_/_/   \___/_/ /_/\__,_/_____/\___/_/ /_/\___/_/ /_/_/ /_/ /_/\__,_/_/  /_/|_|
```

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)
[![Google Benchmark](https://img.shields.io/badge/Google-Benchmark-red.svg)](https://github.com/google/benchmark)

# Arena Benchmark

**Arena Benchmark** is a C++ performance testing framework built on top of Google Benchmark, featuring beautiful table visualization and JSON export capabilities. It makes benchmark results more readable and professional.

## ✨ Features

- 🎨 **Beautiful Table Output** - Unicode borders with color highlighting for best results
- 📊 **Dual Views** - Detailed repetition logs and summary statistics tables
- 📈 **Rich Metrics** - Automatic calculation of average time, throughput, time per item, etc.
- 💾 **JSON Export** - Export results as structured JSON files for further analysis
- 🔥 **Warm-up Support** - Support warm-up runs to exclude cold-start effects
- 🎯 **Fluent API** - Method chaining for clean and elegant code
- 🔧 **Flexible Configuration** - Customizable repetitions, time units, workload scales, etc.

## ⚠️ Important Notes

- **Single-threaded Only**: ArenaBenchmark is NOT thread-safe. All benchmarks must be registered and executed from a single thread. For multi-threaded benchmarking, use Google Benchmark's native API directly.
- **Workload Scale**: Always set `.workload_scale()` when using derived metrics (items/sec, time/item) to ensure accurate calculations.

## 📦 Dependencies

The project uses CMake FetchContent to automatically manage dependencies:

- [Google Benchmark](https://github.com/google/benchmark) - Core performance testing framework
- [nlohmann/json](https://github.com/nlohmann/json) - JSON serialization
- [tabulate](https://github.com/p-ranav/tabulate) - Table visualization
- [fmt](https://github.com/fmtlib/fmt) - Formatting library
- [termcolor](https://github.com/ikalnytskyi/termcolor) - Terminal color support

## 🚀 Quick Start

### Using as a Third-Party Library

Arena Benchmark is a header-only library that can be easily integrated into your project using CMake FetchContent.

#### Method 1: CMake FetchContent (Recommended)

Add the following to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
    arena_benchmark
    GIT_REPOSITORY https://github.com/WittenYeh/arena-benchmark.git
    GIT_TAG main  # or specify a specific tag/commit
)

FetchContent_MakeAvailable(arena_benchmark)

# Link to your target
target_link_libraries(your_target PRIVATE arena_benchmark)
```

#### Method 2: Git Submodule

```bash
# Add as submodule
git submodule add https://github.com/WittenYeh/arena-benchmark.git third_party/arena-benchmark
git submodule update --init --recursive

# In your CMakeLists.txt
add_subdirectory(third_party/arena-benchmark)
target_link_libraries(your_target PRIVATE arena_benchmark)
```

#### Method 3: Manual Installation

```bash
# Clone the repository
git clone https://github.com/WittenYeh/arena-benchmark.git
cd arena-benchmark

# Copy headers to your include path
cp -r include/arena_benchmark /usr/local/include/

# In your CMakeLists.txt, manually link dependencies:
# benchmark, nlohmann_json, tabulate, fmt, termcolor
```

**Note:** Arena Benchmark automatically manages its dependencies (Google Benchmark, nlohmann/json, tabulate, fmt, termcolor) via CMake FetchContent, so you don't need to install them separately.

### Build the Project

```bash
./build.sh
```

### Basic Example

```cpp
#include <arena_benchmark/arena_benchmark.hpp>
#include <benchmark/benchmark.h>
#include <vector>
#include <algorithm>

using namespace arena_benchmark;

int main(int argc, char** argv) {
    ArenaBenchmark bench;

    std::vector<int> data(10000);
    std::iota(data.begin(), data.end(), 0);
    std::shuffle(data.begin(), data.end(), std::mt19937{42});

    // Register benchmark
    bench.register_benchmark("BM_StdSort",
        [&data](benchmark::State& state) {
            for (auto _ : state) {
                auto copy = data;
                std::sort(copy.begin(), copy.end());
                benchmark::DoNotOptimize(copy);
            }
        })
        .repetitions(10)                         // Repeat 10 times (>5 for trimmed avg)
        .workload_scale(10000)                   // Workload scale
        .time_unit(benchmark::kMicrosecond)      // Time unit
        .extra_info("std::sort / 10K");          // Extra info

    // Method chaining: enable trimmed avg -> warm up -> run -> export
    bench.trimmed_avg_enabled(true)              // Enable trimmed average
         .warm_up(1)
         .run_all(argc, argv)
         .export_results(".");

    return 0;
}
```

## 📖 API Documentation

### ArenaBenchmark Class

The main benchmark management class.

#### Methods

##### `register_benchmark(name, func)`

Register a benchmark test.

**Parameters:**
- `name` (string) - Benchmark name
- `func` (callable) - Test function that accepts `benchmark::State&` parameter

**Returns:** `InstanceRegistration&` - For method chaining configuration

**Example:**
```cpp
bench.register_benchmark("BM_MyTest",
    [](benchmark::State& state) {
        for (auto _ : state) {
            // Test code
        }
    });
```

##### `warm_up(num_warm_up_reps)`

Set the number of warm-up runs. Warm-up results are shown in the repetition log but excluded from summary statistics.

**Parameters:**
- `num_warm_up_reps` (int) - Number of warm-up runs, must be >= 0

**Returns:** `ArenaBenchmark&` - Supports method chaining

##### `trimmed_avg_enabled(enabled)`

Enable or disable trimmed average calculation. When enabled, the average is calculated by removing the minimum and maximum values before averaging.

**Parameters:**
- `enabled` (bool) - Enable (true) or disable (false) trimmed average

**Requirements:**
- Only effective when repetitions > 5
- If repetitions ≤ 5, a yellow warning will be displayed and regular average will be used instead

**Returns:** `ArenaBenchmark&` - Supports method chaining

**Example:**
```cpp
bench.register_benchmark("BM_Test", test_func)
     .repetitions(10);  // Must be > 5 for trimmed average

bench.trimmed_avg_enabled(true)  // Enable trimmed average
     .warm_up(1)
     .run_all();
```

##### `run_all()` / `run_all(argc, argv)`

Run all registered benchmarks.

**Parameters:**
- `argc`, `argv` (optional) - Command-line arguments for Google Benchmark configuration

**Returns:** `ArenaBenchmark&` - Supports method chaining

##### `export_results(output_root)`

Export results as JSON files.

**Parameters:**
- `output_root` (path) - Output root directory, defaults to "."

**Output Files:**
- `<output_root>/benchmark_logs/benchmark_repetition_log_board.json` - Detailed logs
- `<output_root>/benchmark_results/benchmark_summary_board.json` - Summary statistics

**Returns:** `ArenaBenchmark&` - Supports method chaining

### InstanceRegistration Class

Benchmark configuration builder returned by `register_benchmark()`.

#### Methods

##### `repetitions(n)`

Set the number of repetitions for averaging.

**Parameters:**
- `n` (int) - Number of repetitions

**Returns:** `InstanceRegistration&`

##### `workload_scale(scale)`

Set the workload scale for calculating derived metrics:
- **items/sec** = workload_scale / real_time_per_iteration
- **avg time/item** = real_time_per_iteration / workload_scale

**Parameters:**
- `scale` (size_t) - Workload scale, must be >= 1

**Returns:** `InstanceRegistration&`

**Example:**
```cpp
// Sorting 10000 elements
bench.register_benchmark("BM_Sort", ...)
     .workload_scale(10000);  // Will display "X us/item" and "Y items/sec"
```

##### `time_unit(unit)`

Set the time unit.

**Parameters:**
- `unit` (benchmark::TimeUnit) - Options:
  - `benchmark::kNanosecond`
  - `benchmark::kMicrosecond`
  - `benchmark::kMillisecond`
  - `benchmark::kSecond`

**Returns:** `InstanceRegistration&`

##### `extra_info(info)`

Set extra information string displayed in result tables.

**Parameters:**
- `info` (string) - Description text

**Returns:** `InstanceRegistration&`

## 📊 Output Format

### 1. Repetition Log Board

Shows detailed results for each run:

![Benchmark Repetition Log Board](assets/benchmark_repetition_log_board.png)

**Column Descriptions:**
- **Benchmark** - Test name
- **Rep#** - Repetition index
- **Iters** - Number of iterations automatically determined by Google Benchmark
- **Real Time** - Actual wall-clock time
- **CPU Time** - CPU time
- **Avg Time/Item** - Average time per item (= Real Time / workload_scale)
- **Items/sec** - Items processed per second (= workload_scale / Real Time)
- **WarmUp** - Whether this is a warm-up run (Y/N)
- **Extra Info** - Additional information

### 2. Benchmark Summary Board

Shows averaged results across multiple runs with highlighting for best and second-best:

![Benchmark Summary Board](assets/benchmark_summary_board.png)

**Highlighting Rules:**
- 🔴 **Red Background** - Best result (shortest Real Time / highest Items/sec)
- 🟡 **Yellow Background** - Second-best result

### 3. JSON Export

#### benchmark_repetition_log_board.json

```json
{
  "board_name": "Benchmark Repetition Log Board",
  "generated_at_unix_ms": 1772286059059,
  "row_count": 20,
  "rows": [
    {
      "avg_time_per_item": 8.826324256811041e-06,
      "cpu_time": 0.008825673018968269,
      "extra_info": "std::sort / 1K",
      "instance_name": "BM_StdSort_Small",
      "is_warm_up": true,
      "items_per_second": 113297446,
      "iterations": 78974,
      "real_time": 0.008826324256811041,
      "repetition_index": 0,
      "time_unit": "ms"
    }
  ]
}
```

#### benchmark_summary_board.json

```json
{
  "board_name": "Benchmark Summary Board",
  "generated_at_unix_ms": 1772286059059,
  "group_by_key": "",
  "row_count": 5,
  "rows": [
    {
      "avg_cpu_time": 0.0085147904500946,
      "avg_items_per_second": 117711092.0,
      "avg_real_time": 0.008515293390783822,
      "avg_time_per_item": 8.515293390783823e-06,
      "extra_info": "std::sort / 1K",
      "instance_name": "BM_StdSort_Small",
      "repetitions": 3,
      "time_unit": "ms"
    }
  ]
}
```

## 🎯 Complete Examples

Check the `examples/` directory for more examples:

- `basic_example.cpp` - Basic usage example
- `test_benchmark_summary.cpp` - Summary table test
- `test_repetition_log_list.cpp` - Repetition log test
- `test_result_exporter.cpp` - JSON export test

Run an example:

```bash
./build/examples/basic_example
```

## 🔧 Advanced Usage

### Custom Workload Scales

```cpp
// Test different data scales
bench.register_benchmark("BM_Sort_1K", ...)
     .workload_scale(1000);

bench.register_benchmark("BM_Sort_10K", ...)
     .workload_scale(10000);

bench.register_benchmark("BM_Sort_100K", ...)
     .workload_scale(100000);
```

### Multiple Repetitions for Better Accuracy

```cpp
bench.register_benchmark("BM_CriticalPath", ...)
     .repetitions(10)  // Run 10 times and average
     .warm_up(2);      // Warm up 2 times
```

### Using Different Time Units

```cpp
// Nanosecond-level test
bench.register_benchmark("BM_FastOp", ...)
     .time_unit(benchmark::kNanosecond);

// Millisecond-level test
bench.register_benchmark("BM_SlowOp", ...)
     .time_unit(benchmark::kMillisecond);
```

### Passing Google Benchmark Arguments

```bash
# Run specific tests
./build/examples/basic_example --benchmark_filter=BM_StdSort.*

# Set minimum run time
./build/examples/basic_example --benchmark_min_time=1.0

# Show verbose output
./build/examples/basic_example --benchmark_verbose
```

## 📁 Project Structure

```
arena-benchmark/
├── include/
│   └── arena_benchmark/
│       ├── arena_benchmark.hpp          # Main entry point
│       ├── instance_registration.hpp    # Configuration builder
│       ├── benchmark_results.hpp        # Result data structures
│       ├── repetition_log_list.hpp      # Repetition log list
│       ├── summary_table.hpp            # Summary table
│       ├── result_exporter.hpp          # JSON exporter
│       └── *_visualizer.hpp             # Visualization components
├── examples/                            # Example code
├── build.sh                             # Build script
└── CMakeLists.txt                       # CMake configuration
```

## 🤝 Contributing

Issues and Pull Requests are welcome!

## 📄 License

This project is licensed under the [MIT License](LICENSE).

## 🙏 Acknowledgments

- [Google Benchmark](https://github.com/google/benchmark) - Core performance testing functionality
- [tabulate](https://github.com/p-ranav/tabulate) - Beautiful table output
- [nlohmann/json](https://github.com/nlohmann/json) - JSON support
