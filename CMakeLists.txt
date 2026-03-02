# --- 1. Project basic configuration ---
cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW) # Use modern timestamp behavior
endif()
project(ArenaBenchmark LANGUAGES C CXX)

# --- Compiler specific Warning Suppression ---
if(CMAKE_CXX_COMPILER_ID MATCHES "IntelLLVM")
	add_compile_options(-Wno-unknown-warning-option)
	add_compile_options(-diag-disable=10430)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# --- Dependency management ---
include(FetchContent)

FetchContent_Declare(
	nlohmann_json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG "v3.12.0"
)

FetchContent_Declare(
    argparse
	GIT_REPOSITORY https://github.com/p-ranav/argparse.git
	GIT_TAG "v3.2"
)

FetchContent_Declare(
    termcolor
	GIT_REPOSITORY https://github.com/ikalnytskyi/termcolor.git
	GIT_TAG "v2.1.0"
)

FetchContent_Declare(
    fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG "12.1.0"
)

FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG "v1.9.5"
)

FetchContent_Declare(
    tabulate
    GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
    GIT_TAG "v1.5"
)

# --- Google Benchmark Configuration ---
# Disable building tests and examples for Google Benchmark to speed up the process
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark testing" FORCE)
set(BENCHMARK_INSTALL_DOCS OFF CACHE BOOL "Disable benchmark docs" FORCE)

# --- Disable tests for other dependencies ---
set(JSON_BuildTests OFF CACHE BOOL "Disable nlohmann_json tests" FORCE)
set(FMT_TEST OFF CACHE BOOL "Disable fmt tests" FORCE)
set(TABULATE_BUILD_TESTS OFF CACHE BOOL "Disable tabulate tests" FORCE)

# --- Downloads and configures all declared content ---
FetchContent_MakeAvailable(
	nlohmann_json
	argparse
	termcolor
	fmt
	benchmark
	tabulate
)

add_library(arena_benchmark INTERFACE)
target_include_directories(arena_benchmark INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(arena_benchmark INTERFACE -Wno-deprecated-literal-operator)

target_link_libraries(arena_benchmark INTERFACE
    benchmark::benchmark
    tabulate::tabulate
)

target_compile_definitions(arena_benchmark INTERFACE
	# avoid error caused by fmt library
	FMT_USE_CONSTEVAL=0
)

target_link_libraries(arena_benchmark INTERFACE
    nlohmann_json::nlohmann_json
    argparse::argparse
    termcolor
    fmt::fmt
)

# --- Build examples ---
option(BUILD_EXAMPLES "Build example programs" ON)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
