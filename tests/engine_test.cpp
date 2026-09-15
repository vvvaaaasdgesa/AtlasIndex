#include "engine.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {

ItemHandle item(const std::string& name, Category category, double price) {
    return ItemHandle(Item{name, category, price});
}

TEST(EngineTest, MaintainsOneItemPerNameAcrossBothIndexes) {
    Engine engine(2);
    EXPECT_TRUE(engine.add(item("Alpha", Category::Book, 10.0)));
    EXPECT_FALSE(engine.add(item("Alpha", Category::Electronics, 99.0)));
    EXPECT_EQ(engine.size(), 1u);
    ASSERT_NE(engine.findByName("Alpha"), nullptr);
    EXPECT_DOUBLE_EQ(engine.findByName("Alpha")->price(), 10.0);

    std::vector<ItemHandle> ordered;
    engine.listInOrder(ordered);
    ASSERT_EQ(ordered.size(), 1u);
    EXPECT_DOUBLE_EQ(ordered.front().price(), 10.0);
}

TEST(EngineTest, ProvidesInclusiveOrderedNameRanges) {
    Engine engine;
    engine.add(item("Delta", Category::Book, 4.0));
    engine.add(item("Alpha", Category::Book, 1.0));
    engine.add(item("Charlie", Category::Book, 3.0));
    engine.add(item("Bravo", Category::Book, 2.0));

    std::vector<ItemHandle> result;
    engine.rangeByName("Bravo", "Delta", result);
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0].name(), "Bravo");
    EXPECT_EQ(result[1].name(), "Charlie");
    EXPECT_EQ(result[2].name(), "Delta");
}

TEST(EngineTest, ReportsCSVIngestionStatistics) {
    Engine engine(2);
    LoadStats stats;
    const std::string path = std::string(ATLASINDEX_SOURCE_DIR) + "/data/sample_small.csv";
    ASSERT_TRUE(engine.ingestCSV(path, stats));
    EXPECT_EQ(stats.parsed, 3u);
    EXPECT_EQ(stats.inserted, 3u);
    EXPECT_EQ(stats.duplicates, 0u);
    EXPECT_EQ(stats.skipped, 0u);
    EXPECT_EQ(engine.size(), 3u);
}

TEST(EngineTest, MissingCSVDoesNotModifyEngine) {
    Engine engine;
    LoadStats stats;
    EXPECT_FALSE(engine.ingestCSV("missing-file.csv", stats));
    EXPECT_EQ(engine.size(), 0u);
}

} // namespace
