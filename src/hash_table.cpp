#include "hash_table.h"

HashTable::HashTable(size_t bucketCount) : buckets_(bucketCount ? bucketCount : 1) {}

bool HashTable::insert(const std::string& key, const ItemHandle& value) {
    // Update an existing key in place so the table size does not change.
    for (auto& node : buckets_[indexFor(key)]) {
        if (node.first == key) {
            node.second = value;
            return false;
        }
    }

    if (size_ + 1 > static_cast<size_t>(buckets_.size() * maxLoadFactor_)) {
        size_t target = buckets_.size() * 2;
        const size_t required = minimumBucketsFor(size_ + 1);
        if (target < required) target = required;
        rehash(target);
    }

    auto& bucket = buckets_[indexFor(key)];
    bucket.emplace_back(key, value);
    ++size_;
    return true;
}

void HashTable::reserve(size_t elementCount) {
    const size_t required = minimumBucketsFor(elementCount);
    if (required > buckets_.size()) rehash(required);
}

void HashTable::rehash(size_t bucketCount) {
    const size_t required = minimumBucketsFor(size_);
    if (bucketCount < required) bucketCount = required;
    if (bucketCount == 0) bucketCount = 1;
    if (bucketCount == buckets_.size()) return;

    std::vector<std::list<Node>> newBuckets(bucketCount);
    for (auto& bucket : buckets_) {
        while (!bucket.empty()) {
            auto node = bucket.begin();
            const size_t index = std::hash<std::string>{}(node->first) % bucketCount;
            newBuckets[index].splice(newBuckets[index].end(), bucket, node);
        }
    }
    buckets_.swap(newBuckets);
}

float HashTable::load_factor() const {
    return buckets_.empty() ? 0.0f
                            : static_cast<float>(size_) / buckets_.size();
}

void HashTable::max_load_factor(float value) {
    if (!std::isfinite(value) || value <= 0.0f) return;
    maxLoadFactor_ = value;
    const size_t required = minimumBucketsFor(size_);
    if (required > buckets_.size()) rehash(required);
}

ItemHandle* HashTable::find(const std::string& key) {
    auto& bucket = buckets_[indexFor(key)];
    for (auto& node : bucket) {
        if (node.first == key) return &node.second;
    }
    return nullptr;
}

const ItemHandle* HashTable::find(const std::string& key) const {
    const auto& bucket = buckets_[indexFor(key)];
    for (const auto& node : bucket) {
        if (node.first == key) return &node.second;
    }
    return nullptr;
}

bool HashTable::erase(const std::string& key) {
    auto& bucket = buckets_[indexFor(key)];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->first == key) {
            bucket.erase(it);
            --size_;
            return true;
        }
    }
    return false;
}
