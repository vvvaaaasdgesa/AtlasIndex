#include "engine.h"
#include <iostream>
#include <string>
#include <vector>

namespace {

void printItems(const std::vector<ItemHandle>& items) {
    for (const auto& item : items) std::cout << item << '\n';
}

void printUsage(const char* program) {
    std::cout << "Usage: " << program << " <csv-file> [command]\n"
              << "Commands:\n"
              << "  --list                 Print every item in name order\n"
              << "  --find <name>          Look up one item by exact name\n"
              << "  --range <first> <last> Print an inclusive name range\n";
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    Engine engine(16);
    LoadStats stats;
    if (!engine.ingestCSV(argv[1], stats)) {
        std::cerr << "Could not open CSV file: " << argv[1] << '\n';
        return 1;
    }

    std::cout << "Parsed: " << stats.parsed
              << " | Inserted: " << stats.inserted
              << " | Duplicates: " << stats.duplicates
              << " | Skipped: " << stats.skipped
              << " | Hash buckets: " << engine.hashBucketCount() << '\n';

    if (argc == 2 || (argc == 3 && std::string(argv[2]) == "--list")) {
        std::vector<ItemHandle> items;
        engine.listInOrder(items);
        printItems(items);
        return 0;
    }

    const std::string command = argv[2];
    if (command == "--find" && argc == 4) {
        const ItemHandle* item = engine.findByName(argv[3]);
        if (!item) {
            std::cout << "Not found\n";
            return 2;
        }
        std::cout << *item << '\n';
        return 0;
    }

    if (command == "--range" && argc == 5) {
        std::vector<ItemHandle> items;
        engine.rangeByName(argv[3], argv[4], items);
        printItems(items);
        return 0;
    }

    printUsage(argv[0]);
    return 1;
}
