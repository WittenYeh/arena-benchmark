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

**Arena Benchmark** 是一个基于 Google Benchmark 的 C++ 性能测试框架，提供美观的表格可视化和 JSON 导出功能。它让性能测试结果更易读、更专业。

[English](README.md) | 简体中文

## ✨ 特性

- 🎨 **美观的表格输出** - 使用 Unicode 边框和颜色高亮显示最佳结果
- 📊 **双重视图** - 提供详细的重复日志和汇总统计表
- 📈 **丰富的指标** - 自动计算平均时间、吞吐量、每项耗时等
- 💾 **JSON 导出** - 将结果导出为结构化 JSON 文件，便于后续分析
- 🔥 **预热支持** - 支持预热运行，排除冷启动影响
- 🎯 **流式 API** - 链式调用，代码简洁优雅
- 🔧 **灵活配置** - 支持自定义重复次数、时间单位、工作负载规模等

## 📦 依赖

项目使用 CMake FetchContent 自动管理依赖：

- [Google Benchmark](https://github.com/google/benchmark) - 核心性能测试框架
- [nlohmann/json](https://github.com/nlohmann/json) - JSON 序列化
- [tabulate](https://github.com/p-ranav/tabulate) - 表格可视化
- [fmt](https://github.com/fmtlib/fmt) - 格式化输出
- [termcolor](https://github.com/ikalnytskyi/termcolor) - 终端颜色支持

## 🚀 快速开始

### 构建项目

```bash
./build.sh
```

### 基础示例

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

    // 注册基准测试
    bench.register_benchmark("BM_StdSort",
        [&data](benchmark::State& state) {
            for (auto _ : state) {
                auto copy = data;
                std::sort(copy.begin(), copy.end());
                benchmark::DoNotOptimize(copy);
            }
        })
        .repetitions(3)                          // 重复 3 次
        .workload_scale(10000)                   // 工作负载规模
        .time_unit(benchmark::kMicrosecond)      // 时间单位
        .extra_info("std::sort / 10K");          // 额外信息

    // 链式调用：预热 -> 运行 -> 导出
    bench.warm_up(1)
         .run_all(argc, argv)
         .export_results(".");

    return 0;
}
```

## 📖 API 文档

### ArenaBenchmark 类

主要的基准测试管理类。

#### 方法

##### `register_benchmark(name, func)`

注册一个基准测试。

**参数：**
- `name` (string) - 基准测试名称
- `func` (callable) - 测试函数，接受 `benchmark::State&` 参数

**返回：** `InstanceRegistration&` - 用于链式配置

**示例：**
```cpp
bench.register_benchmark("BM_MyTest",
    [](benchmark::State& state) {
        for (auto _ : state) {
            // 测试代码
        }
    });
```

##### `warm_up(num_warm_up_reps)`

设置预热运行次数。预热结果会显示在重复日志中，但不计入汇总统计。

**参数：**
- `num_warm_up_reps` (int) - 预热次数，必须 >= 0

**返回：** `ArenaBenchmark&` - 支持链式调用

##### `run_all()` / `run_all(argc, argv)`

运行所有注册的基准测试。

**参数：**
- `argc`, `argv` (可选) - 命令行参数，用于 Google Benchmark 配置

**返回：** `ArenaBenchmark&` - 支持链式调用

##### `export_results(output_root)`

将结果导出为 JSON 文件。

**参数：**
- `output_root` (path) - 输出根目录，默认为 "."

**输出文件：**
- `<output_root>/benchmark_logs/benchmark_repetition_log_board.json` - 详细日志
- `<output_root>/benchmark_results/benchmark_summary_board.json` - 汇总统计

**返回：** `ArenaBenchmark&` - 支持链式调用

### InstanceRegistration 类

基准测试配置构建器，由 `register_benchmark()` 返回。

#### 方法

##### `repetitions(n)`

设置重复运行次数，用于计算平均值。

**参数：**
- `n` (int) - 重复次数

**返回：** `InstanceRegistration&`

##### `workload_scale(scale)`

设置工作负载规模，用于计算派生指标：
- **items/sec** = workload_scale / real_time_per_iteration
- **avg time/item** = real_time_per_iteration / workload_scale

**参数：**
- `scale` (size_t) - 工作负载规模，必须 >= 1

**返回：** `InstanceRegistration&`

**示例：**
```cpp
// 对 10000 个元素排序
bench.register_benchmark("BM_Sort", ...)
     .workload_scale(10000);  // 将显示 "X us/item" 和 "Y items/sec"
```

##### `time_unit(unit)`

设置时间单位。

**参数：**
- `unit` (benchmark::TimeUnit) - 可选值：
  - `benchmark::kNanosecond`
  - `benchmark::kMicrosecond`
  - `benchmark::kMillisecond`
  - `benchmark::kSecond`

**返回：** `InstanceRegistration&`

##### `extra_info(info)`

设置额外信息字符串，显示在结果表格中。

**参数：**
- `info` (string) - 描述信息

**返回：** `InstanceRegistration&`

## 📊 输出格式

### 1. Repetition Log Board（重复日志表）

显示每次运行的详细结果：

```
·────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────·
│                                                             Benchmark Repetition Log Board                                                             │
·────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────·
│      Benchmark      │ Rep# │  Iters │     Real Time    │     CPU Time     │     Avg Time/Item    │     Items/sec    │ WarmUp │        Extra Info       │
·─────────────────────·──────·────────·──────────────────·──────────────────·──────────────────────·──────────────────·────────·─────────────────────────·
│   BM_StdSort_Small  │   0  │  78974 │      0.01 ms     │      0.01 ms     │    0.0000 ms/item    │     113297446    │    Y   │      std::sort / 1K     │
│   BM_StdSort_Small  │   0  │  80220 │      0.01 ms     │      0.01 ms     │    0.0000 ms/item    │     114149328    │    N   │      std::sort / 1K     │
```

**列说明：**
- **Benchmark** - 测试名称
- **Rep#** - 重复索引
- **Iters** - Google Benchmark 自动确定的迭代次数
- **Real Time** - 实际运行时间
- **CPU Time** - CPU 时间
- **Avg Time/Item** - 每项平均耗时（= Real Time / workload_scale）
- **Items/sec** - 每秒处理项数（= workload_scale / Real Time）
- **WarmUp** - 是否为预热运行（Y/N）
- **Extra Info** - 额外信息

### 2. Benchmark Summary Board（汇总统计表）

显示多次运行的平均结果，并高亮最佳和次佳：

```
╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                                          Benchmark Summary Board                                                         ║
║ ·─────────────────────·──────·──────────────────·──────────────────·──────────────────────·──────────────────·─────────────────────────────────────· ║
║ │      Benchmark      │ Reps │     Real Time    │     CPU Time     │     Avg Time/Item    │     Items/sec    │        Extra Info       │ ║
║ ·─────────────────────·──────·──────────────────·──────────────────·──────────────────────·──────────────────·─────────────────────────────────────· ║
║ │   BM_StdSort_Small  │   3  │      0.01 ms     │      0.01 ms     │    0.0000 ms/item    │  117,711,092.00  │      std::sort / 1K     │ ║
╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
```

**高亮规则：**
- 🔴 **红色背景** - 最佳结果（Real Time 最短 / Items/sec 最高）
- 🟡 **黄色背景** - 次佳结果

### 3. JSON 导出

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

## 🎯 完整示例

查看 `examples/` 目录获取更多示例：

- `basic_example.cpp` - 基础用法示例
- `test_benchmark_summary.cpp` - 汇总表测试
- `test_repetition_log_list.cpp` - 重复日志测试
- `test_result_exporter.cpp` - JSON 导出测试

运行示例：

```bash
./build/examples/basic_example
```

## 🔧 高级用法

### 自定义工作负载规模

```cpp
// 测试不同规模的数据
bench.register_benchmark("BM_Sort_1K", ...)
     .workload_scale(1000);

bench.register_benchmark("BM_Sort_10K", ...)
     .workload_scale(10000);

bench.register_benchmark("BM_Sort_100K", ...)
     .workload_scale(100000);
```

### 多次重复以提高准确性

```cpp
bench.register_benchmark("BM_CriticalPath", ...)
     .repetitions(10)  // 运行 10 次取平均
     .warm_up(2);      // 预热 2 次
```

### 使用不同时间单位

```cpp
// 纳秒级测试
bench.register_benchmark("BM_FastOp", ...)
     .time_unit(benchmark::kNanosecond);

// 毫秒级测试
bench.register_benchmark("BM_SlowOp", ...)
     .time_unit(benchmark::kMillisecond);
```

### 传递 Google Benchmark 参数

```bash
# 运行特定测试
./build/examples/basic_example --benchmark_filter=BM_StdSort.*

# 设置最小运行时间
./build/examples/basic_example --benchmark_min_time=1.0

# 显示详细输出
./build/examples/basic_example --benchmark_verbose
```

## 📁 项目结构

```
arena-benchmark/
├── include/
│   └── arena_benchmark/
│       ├── arena_benchmark.hpp          # 主入口
│       ├── instance_registration.hpp    # 配置构建器
│       ├── benchmark_results.hpp        # 结果数据结构
│       ├── repetition_log_list.hpp      # 重复日志列表
│       ├── summary_table.hpp            # 汇总表
│       ├── result_exporter.hpp          # JSON 导出器
│       └── *_visualizer.hpp             # 可视化组件
├── examples/                            # 示例代码
├── build.sh                             # 构建脚本
└── CMakeLists.txt                       # CMake 配置
```

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

## 📄 许可证

本项目采用 [MIT License](LICENSE) 许可证。

## 🙏 致谢

- [Google Benchmark](https://github.com/google/benchmark) - 提供核心性能测试功能
- [tabulate](https://github.com/p-ranav/tabulate) - 提供美观的表格输出
- [nlohmann/json](https://github.com/nlohmann/json) - 提供 JSON 支持
