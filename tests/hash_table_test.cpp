#include "hash_table.h"
#include <gtest/gtest.h>
#include <string>

namespace {

ItemHandle makeItem(const std::string& name, double price = 1.0) {
    return ItemHandle(Item{name, Category::Book, price});
}

TEST(HashTableTest, RehashesAutomaticallyAndPreservesEntries) {
    HashTable table(2);
    const size_t originalBuckets = table.bucket_count();

    for (int i = 0; i < 200; ++i) {
        const std::string key = "key-" + std::to_string(i);
        EXPECT_TRUE(table.insert(key, makeItem(key, i)));
    }

    EXPECT_GT(table.bucket_count(), originalBuckets);
    EXPECT_LE(table.load_factor(), table.max_load_factor());
    EXPECT_EQ(table.size(), 200u);
    for (int i = 0; i < 200; ++i) {
        const std::string key = "key-" + std::to_string(i);
        const ItemHandle* found = table.find(key);
        ASSERT_NE(found, nullptr);
        EXPECT_DOUBLE_EQ(found->price(), i);
    }
}

TEST(HashTableTest, UpdateDoesNotGrowOrChangeSize) {
    HashTable table(8);
    EXPECT_TRUE(table.insert("same", makeItem("same", 1.0)));
    const size_t buckets = table.bucket_count();
    EXPECT_FALSE(table.insert("same", makeItem("same", 9.0)));
    EXPECT_EQ(table.size(), 1u);
    EXPECT_EQ(table.bucket_count(), buckets);
    ASSERT_NE(table.find("same"), nullptr);
    EXPECT_DOUBLE_EQ(table.find("same")->price(), 9.0);
}

TEST(HashTableTest, ReserveAndManualRehashKeepData) {
    HashTable table(2);
    table.insert("alpha", makeItem("alpha"));
    table.reserve(100);
    EXPECT_GE(table.bucket_count(), 134u);
    EXPECT_NE(table.find("alpha"), nullptr);

    table.rehash(1);
    EXPECT_GE(table.bucket_count(), 2u);
    EXPECT_NE(table.find("alpha"), nullptr);
}

TEST(HashTableTest, HonorsAConfiguredMaximumLoadFactor) {
    HashTable table(1);
    table.max_load_factor(0.1f);
    table.insert("alpha", makeItem("alpha"));
    EXPECT_LE(table.load_factor(), table.max_load_factor());

    const float before = table.max_load_factor();
    table.max_load_factor(-1.0f);
    EXPECT_EQ(table.max_load_factor(), before);
}

} // namespace
