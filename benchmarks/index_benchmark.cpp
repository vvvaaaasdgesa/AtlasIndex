#include "bst.h"
#include "hash_table.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

template <class Operation>
long long timeMicros(Operation operation) {
    const auto start = Clock::now();
    operation();
    return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start).count();
}

std::vector<ItemHandle> makeItems(size_t count) {
    std::vector<ItemHandle> items;
    items.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        items.emplace_back(Item{"item-" + std::to_string(i), Category::Book,
                                static_cast<double>(i % 1000)});
    }
    return items;
}

void printRow(const char* operation, long long hashTime,
              long long bstTime, long long vectorTime) {
    std::cout << std::left << std::setw(12) << operation
              << std::right << std::setw(14) << hashTime
              << std::setw(14) << bstTime
              << std::setw(14) << vectorTime << '\n';
}

} // namespace

int main(int argc, char* argv[]) {
    const size_t count = argc > 1 ? std::strtoull(argv[1], nullptr, 10) : 5000;
    auto items = makeItems(count);
    std::mt19937 generator(32);
    std::shuffle(items.begin(), items.end(), generator);

    HashTable hash(16);
    BST tree;
    std::vector<ItemHandle> linear;
    linear.reserve(count);

    const auto hashInsert = timeMicros([&] {
        for (const auto& value : items) hash.insert(value.name(), value);
    });
    const auto bstInsert = timeMicros([&] {
        for (const auto& value : items) tree.insert(value);
    });
    const auto vectorInsert = timeMicros([&] {
        for (const auto& value : items) linear.push_back(value);
    });

    volatile size_t matches = 0;
    const auto hashLookup = timeMicros([&] {
        for (const auto& value : items) matches += hash.find(value.name()) != nullptr;
    });
    const auto bstLookup = timeMicros([&] {
        for (const auto& value : items) matches += tree.contains(value);
    });
    const auto vectorLookup = timeMicros([&] {
        for (const auto& value : items) {
            matches += std::find_if(linear.begin(), linear.end(), [&](const ItemHandle& candidate) {
                return candidate.name() == value.name();
            }) != linear.end();
        }
    });

    const auto hashErase = timeMicros([&] {
        for (const auto& value : items) hash.erase(value.name());
    });
    const auto bstErase = timeMicros([&] {
        for (const auto& value : items) tree.erase(value);
    });
    const auto vectorErase = timeMicros([&] {
        for (const auto& value : items) {
            const auto found = std::find_if(linear.begin(), linear.end(), [&](const ItemHandle& candidate) {
                return candidate.name() == value.name();
            });
            if (found != linear.end()) linear.erase(found);
        }
    });

    std::cout << "AtlasIndex benchmark (" << count << " items, microseconds)\n";
    std::cout << std::left << std::setw(12) << "Operation"
              << std::right << std::setw(14) << "Hash table"
              << std::setw(14) << "BST"
              << std::setw(14) << "Vector" << '\n';
    printRow("insert", hashInsert, bstInsert, vectorInsert);
    printRow("lookup", hashLookup, bstLookup, vectorLookup);
    printRow("erase", hashErase, bstErase, vectorErase);
    std::cout << "Lookup matches: " << matches << '\n';
}
