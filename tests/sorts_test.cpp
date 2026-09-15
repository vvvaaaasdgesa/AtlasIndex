#include "sorts.h"
#include <gtest/gtest.h>
#include <vector>

namespace {

std::vector<ItemHandle> input() {
    return {
        ItemHandle(Item{"C", Category::Book, 3.0}),
        ItemHandle(Item{"A", Category::Book, 1.0}),
        ItemHandle(Item{"B", Category::Book, 2.0})
    };
}

void expectNames(const std::vector<ItemHandle>& items,
                 const std::vector<const char*>& names) {
    ASSERT_EQ(items.size(), names.size());
    for (size_t i = 0; i < names.size(); ++i) EXPECT_EQ(items[i].name(), names[i]);
}

TEST(SortsTest, InsertionSupportsBothDirections) {
    auto ascending = input();
    insertionSortItems(ascending, true);
    expectNames(ascending, {"A", "B", "C"});

    auto descending = input();
    insertionSortItems(descending, false);
    expectNames(descending, {"C", "B", "A"});
}

TEST(SortsTest, MergeSupportsBothDirections) {
    auto ascending = input();
    mergeSortItems(ascending, true);
    expectNames(ascending, {"A", "B", "C"});

    auto descending = input();
    mergeSortItems(descending, false);
    expectNames(descending, {"C", "B", "A"});
}

} // namespace
