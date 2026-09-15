#pragma once
#include "item.h"
#include <vector>
#include <list>
#include <string>
#include <utility>
#include <functional>
#include <cmath>

class HashTable {
public:
    explicit HashTable(size_t bucketCount = 256);

    bool               insert(const std::string& key, const ItemHandle& value); // true=new, false=updated
    ItemHandle*        find(const std::string& key);
    const ItemHandle*  find(const std::string& key) const;
    bool               erase(const std::string& key);

    void               reserve(size_t elementCount);
    void               rehash(size_t bucketCount);
    float              load_factor() const;
    float              max_load_factor() const { return maxLoadFactor_; }
    void               max_load_factor(float value);

    size_t             size() const { return size_; }
    size_t             bucket_count() const { return buckets_.size(); }
    size_t             bucket_size(size_t i) const { return i < buckets_.size() ? buckets_[i].size() : 0; }

private:
    using Node = std::pair<std::string, ItemHandle>;
    std::vector<std::list<Node>> buckets_;
    size_t size_{0};
    float maxLoadFactor_{0.75f};

    size_t indexFor(const std::string& key) const {
        return buckets_.empty() ? 0u : std::hash<std::string>{}(key) % buckets_.size();
    }

    size_t minimumBucketsFor(size_t elementCount) const {
        return static_cast<size_t>(std::ceil(elementCount / maxLoadFactor_));
    }
};
