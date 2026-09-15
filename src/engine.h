#pragma once
#include "item.h"
#include "bst.h"
#include "hash_table.h"
#include "sorts.h"
#include <string>
#include <vector>

struct LoadStats {
    size_t parsed{0};
    size_t inserted{0};
    size_t duplicates{0};
    size_t skipped{0};
};

class Engine {
public:
    explicit Engine(size_t hashBuckets = 256);

    void loadCSV(const std::string& path);           // prints summary internally
    bool ingestCSV(const std::string& path, LoadStats& stats);
    bool add(const ItemHandle& item);                // one item per name; reject an existing name
    bool eraseByName(const std::string& name);       // remove the name from both structures
    const ItemHandle* findByName(const std::string& name) const;

    void listInOrder(std::vector<ItemHandle>& out) const;
    void rangeByName(const std::string& first, const std::string& last,
                     std::vector<ItemHandle>& out) const;
    void sortSnapshot(std::vector<ItemHandle>& out, SortAlgo algo, bool ascending) const;

    size_t size() const { return byName_.size(); }
    size_t hashBucketCount() const { return byName_.bucket_count(); }
    float hashLoadFactor() const { return byName_.load_factor(); }

private:
    HashTable byName_;  // key: item name
    BST       ordered_; // ordered by operator<
};
