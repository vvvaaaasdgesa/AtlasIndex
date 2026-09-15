cmake_minimum_required(VERSION 3.16)
project(AtlasIndex VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ATLASINDEX_BUILD_TESTS "Build the GoogleTest suite" ON)
option(ATLASINDEX_BUILD_BENCHMARKS "Build the performance benchmark" ON)

add_library(atlasindex_core
    src/bst.cpp
    src/engine.cpp
    src/hash_table.cpp
    src/item.cpp
    src/csv_utils.cpp
    src/sorts.cpp
)
add_library(AtlasIndex::core ALIAS atlasindex_core)
target_include_directories(atlasindex_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(atlasindex_core PRIVATE /W4)
else()
    target_compile_options(atlasindex_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(atlasindex_cli app/atlasindex_cli.cpp)
target_link_libraries(atlasindex_cli PRIVATE AtlasIndex::core)

add_executable(atlasindex_regression tests/regression_runner.cpp)
target_link_libraries(atlasindex_regression PRIVATE AtlasIndex::core)

if(ATLASINDEX_BUILD_BENCHMARKS)
    add_executable(atlasindex_bench benchmarks/index_benchmark.cpp)
    target_link_libraries(atlasindex_bench PRIVATE AtlasIndex::core)
endif()

if(ATLASINDEX_BUILD_TESTS)
    include(CTest)
    include(FetchContent)

    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    add_executable(atlasindex_tests
        tests/bst_test.cpp
        tests/engine_test.cpp
        tests/hash_table_test.cpp
        tests/sorts_test.cpp
    )
    target_link_libraries(atlasindex_tests PRIVATE AtlasIndex::core GTest::gtest_main)
    target_compile_definitions(atlasindex_tests PRIVATE
        ATLASINDEX_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )

    include(GoogleTest)
    gtest_discover_tests(atlasindex_tests)
    add_test(NAME regression_suite COMMAND atlasindex_regression)
    set_tests_properties(regression_suite PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
