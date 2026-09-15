#include "bst.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {

ItemHandle item(const std::string& name, double price = 1.0) {
    return ItemHandle(Item{name, Category::Book, price});
}

TEST(BSTTest, InorderAndInclusiveNameRangeAreSorted) {
    BST tree;
    for (const char* name : {"M", "F", "T", "B", "H", "R", "Z"}) {
        tree.insert(item(name));
    }

    std::vector<ItemHandle> result;
    tree.rangeByName("F", "T", result);
    ASSERT_EQ(result.size(), 5u);
    EXPECT_EQ(result[0].name(), "F");
    EXPECT_EQ(result[1].name(), "H");
    EXPECT_EQ(result[2].name(), "M");
    EXPECT_EQ(result[3].name(), "R");
    EXPECT_EQ(result[4].name(), "T");
}

TEST(BSTTest, ReversedAndEmptyRangesReturnNoItems) {
    BST tree;
    tree.insert(item("M"));
    std::vector<ItemHandle> result{item("old")};

    tree.rangeByName("Z", "A", result);
    EXPECT_TRUE(result.empty());
    tree.rangeByName("N", "Q", result);
    EXPECT_TRUE(result.empty());
}

TEST(BSTTest, ValueRangeUsesFullItemOrdering) {
    BST tree;
    for (const char* name : {"A", "B", "C", "D"}) tree.insert(item(name));
    std::vector<ItemHandle> result;
    tree.range(item("B"), item("C"), result);
    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result.front().name(), "B");
    EXPECT_EQ(result.back().name(), "C");
}

TEST(BSTTest, DeletionKeepsTreeValid) {
    BST tree;
    for (const char* name : {"M", "F", "T", "R", "Z"}) tree.insert(item(name));
    EXPECT_TRUE(tree.erase(item("T")));
    EXPECT_TRUE(tree.isValidBST());
    EXPECT_FALSE(tree.contains(item("T")));
}

} // namespace
