# AtlasIndex

AtlasIndex is a C++17 in-memory catalog engine built around two custom data
structures: a separate-chaining hash table for exact-name lookups and a binary
search tree for ordered and range queries. CSV ingestion updates both indexes
atomically so they always describe the same collection.

Built by Yuvia Liu.

## Features

- Custom separate-chaining hash table with automatic load-factor-based rehashing
- Custom BST with insertion, search, validation, deletion, inorder traversal,
  and inclusive range queries
- Exact-name lookup in expected O(1) time
- Ordered listing and name-range queries in sorted order
- CSV ingestion with parsed, inserted, duplicate, and skipped-row statistics
- Stable insertion sort and merge sort implementations
- CMake targets for the CLI, regression runner, GoogleTest suite, and
  performance benchmark
- Benchmarks comparing custom hash-table, BST, and linear-vector insertion,
  lookup, and deletion

## Build

Requirements: a C++17 compiler and CMake 3.16 or newer. CMake uses an installed
GoogleTest package when available; otherwise it downloads GoogleTest 1.15.2 at
configure time.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

To build without tests or benchmarks:

```bash
cmake -S . -B build -DATLASINDEX_BUILD_TESTS=OFF -DATLASINDEX_BUILD_BENCHMARKS=OFF
cmake --build build -j
```

## Run the CLI

```bash
./build/atlasindex_cli data/sample_small.csv --list
./build/atlasindex_cli data/sample_small.csv --find "Hoodie"
./build/atlasindex_cli data/sample_small.csv --range "A" "M"
```

Name ranges are inclusive and returned in ascending BST order.

## Test

```bash
ctest --test-dir build --output-on-failure
```

`atlasindex_regression` provides a readable 32-check end-to-end run, while
`atlasindex_tests` covers rehashing, range boundaries, cross-index consistency,
CSV statistics, deletion, and both sorting directions with GoogleTest.

## Benchmark

```bash
./build/atlasindex_bench       # 5,000 items
./build/atlasindex_bench 20000 # custom data size
```

The benchmark uses a fixed shuffle seed for repeatable insertion order and
reports elapsed microseconds. Results depend on hardware and build type; use a
Release build for meaningful comparisons.

## Architecture

```text
CSV file -> Engine -> HashTable (exact lookup)
                   -> BST       (ordered/range queries)
```

The Engine rejects duplicate names before either index changes. If the second
step of a mutation fails, it rolls back the first step to preserve consistency.

## Complexity

| Operation | Hash table | BST (average) | BST (worst) |
|---|---:|---:|---:|
| Insert | O(1) amortized | O(log n) | O(n) |
| Exact lookup | O(1) expected | O(log n) | O(n) |
| Delete | O(1) expected | O(log n) | O(n) |
| Ordered traversal | - | O(n) | O(n) |
| Range query | - | O(h + k) | O(n) |

`h` is tree height and `k` is the number of returned items.
