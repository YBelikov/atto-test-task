#include <gtest/gtest.h>
#include <thread>
#include "LRUCache.hpp"

class LRUCacheTest : public ::testing::Test {
protected:
    LRUCache cache{3}; // capacity 3
};

TEST_F(LRUCacheTest, BasicSetGet) {
    cache.set("A", "Alpha");
    cache.set("B", "Bravo");
    cache.set("C", "Charlie");

    EXPECT_EQ(cache.get("A"), "Alpha");
    EXPECT_EQ(cache.get("B"), "Bravo");
    EXPECT_EQ(cache.get("C"), "Charlie");
}

TEST_F(LRUCacheTest, LRU_Eviction) {
    cache.set("A", "Alpha");
    cache.set("B", "Bravo");
    cache.set("C", "Charlie");

    cache.set("D", "Delta");  // Evicts "A"

    EXPECT_EQ(cache.get("A"), "");
    EXPECT_EQ(cache.get("B"), "Bravo");
    EXPECT_EQ(cache.get("C"), "Charlie");
    EXPECT_EQ(cache.get("D"), "Delta");
}

TEST_F(LRUCacheTest, AccessUpdatesLRU) {
    cache.set("A", "Alpha");
    cache.set("B", "Bravo");
    cache.set("C", "Charlie");

    cache.get("A");  // "A" is now the most recently used

    cache.set("D", "Delta"); // Evicts "B" instead of "A"

    EXPECT_EQ(cache.get("B"), ""); // "B" should be evicted
    EXPECT_EQ(cache.get("A"), "Alpha");
}

TEST_F(LRUCacheTest, UpdateExistingKey) {
    cache.set("A", "Alpha");
    cache.set("B", "Bravo");

    cache.set("A", "UpdatedAlpha");

    EXPECT_EQ(cache.get("A"), "UpdatedAlpha");
}

TEST_F(LRUCacheTest, RemoveKey) {
    cache.set("A", "Alpha");
    cache.set("B", "Bravo");

    cache.remove("A");

    EXPECT_EQ(cache.get("A"), "");
    EXPECT_EQ(cache.get("B"), "Bravo");
}

TEST_F(LRUCacheTest, CacheClear) {
    cache.set("A", "Alpha");
    cache.set("B", "Bravo");
    cache.clear();

    EXPECT_EQ(cache.get("A"), "");
    EXPECT_EQ(cache.get("B"), "");
}

TEST_F(LRUCacheTest, InsertMoreThanCapacity) {
    cache.set("1", "One");
    cache.set("2", "Two");
    cache.set("3", "Three");
    cache.set("4", "Four");  // Evicts "1"

    EXPECT_EQ(cache.get("1"), ""); // s"1" should be evicted
    EXPECT_EQ(cache.get("2"), "Two");
    EXPECT_EQ(cache.get("3"), "Three");
    EXPECT_EQ(cache.get("4"), "Four");
}

TEST_F(LRUCacheTest, ConcurrentAccess) {
    LRUCache cache(5);

    std::thread writer([&]() {
        for (int i = 0; i < 10; i++) {
            cache.set("Key" + std::to_string(i), "Value" + std::to_string(i));
        }
    });

    std::thread reader([&]() {
        for (int i = 0; i < 10; i++) {
            cache.get("Key" + std::to_string(i));
        }
    });

    writer.join();
    reader.join();

    EXPECT_EQ(cache.get("Key5"), "Value5");
}
