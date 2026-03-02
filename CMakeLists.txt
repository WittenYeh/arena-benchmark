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
	URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
	URL_HASH SHA256=8c4b26bf4b422252e13f332bc5e388ec0ab5c3443d24399acb675e68278d341f
)

FetchContent_Declare(
    argparse
	GIT_REPOSITORY https://github.com/p-ranav/argparse.git
	GIT_TAG "3eda91b2e1ce7d569f84ba295507c4cd8fd96910"
)

FetchContent_Declare(
    termcolor
	GIT_REPOSITORY https://github.com/ikalnytskyi/termcolor.git
	GIT_TAG "b3cb0f365f8435588df7a6b12a82b2ac5fc1fe95"
)

FetchContent_Declare(
    fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG "407c905e45ad75fc29bf0f9bb7c5c2fd3475976f"
)

FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG "192ef10025eb2c4cdd392bc502f0c852196baa48"
)

FetchContent_Declare(
    tabulate
    GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
    GIT_TAG "b35db4cce50a4b296290b0ae827305cdeb23751e"
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
