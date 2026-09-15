#include "engine.h"
#include "csv_utils.h"
#include <iostream>

Engine::Engine(size_t hashBuckets) : byName_(hashBuckets) {}

void Engine::loadCSV(const std::string& path) {
    using csv_utils::now; using csv_utils::ms_between;
    auto t0 = now();
    LoadStats stats;
    if (!ingestCSV(path, stats)) {
        std::cout << "ERROR: cannot open " << path << "\n";
        return;
    }
    auto t1 = now();
    std::cout << "LOADED " << stats.inserted << " (skipped " << stats.skipped << ") in "
              << ms_between(t0, t1) << " ms\n";
}

bool Engine::ingestCSV(const std::string& path, LoadStats& stats) {
    stats = {};
    std::vector<ItemHandle> items;
    if (!csv_utils::readCSV(path, items, stats.skipped)) return false;
    stats.parsed = items.size();
    for (const auto& h : items) {
        if (add(h)) ++stats.inserted;
        else ++stats.duplicates;
    }
    return true;
}

bool Engine::add(const ItemHandle& item) {
    // Reject duplicate names before changing either index.
    if (byName_.find(item.name())) return false;

    ordered_.insert(item);
    // Keep the two indexes in sync if the hash insertion fails.
    if (!byName_.insert(item.name(), item)) {
        ordered_.erase(item);
        return false;
    }
    return true;
}

bool Engine::eraseByName(const std::string& name) {
    const ItemHandle* found = byName_.find(name);
    if (!found) return false;

    // Save a copy before removing the hash entry that owns found.
    ItemHandle item = *found;
    if (!ordered_.erase(item)) return false;
    // Restore the BST if the hash removal fails.
    if (!byName_.erase(name)) {
        ordered_.insert(item);
        return false;
    }
    return true;
}

const ItemHandle* Engine::findByName(const std::string& name) const {
    return byName_.find(name);
}

void Engine::listInOrder(std::vector<ItemHandle>& out) const {
    ordered_.inorder(out);
}

void Engine::rangeByName(const std::string& first, const std::string& last,
                         std::vector<ItemHandle>& out) const {
    ordered_.rangeByName(first, last, out);
}

void Engine::sortSnapshot(std::vector<ItemHandle>& out, SortAlgo algo, bool ascending) const {
    ordered_.inorder(out);
    sortItems(out, algo, ascending);
}
