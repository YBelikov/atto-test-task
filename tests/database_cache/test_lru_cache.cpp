#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "i_db.hpp"

class DatabaseCacheTest : public ::testing::Test {
protected:
    i_db db{5}; // cache size = 5
};

TEST_F(DatabaseCacheTest, BasicSetGet) {
    db.begin_transaction();
    EXPECT_EQ(db.set("A", "Hello"), "OK (Pending transaction)");
    db.commit_transaction();
    EXPECT_EQ(db.get("A"), "Hello");
}

TEST_F(DatabaseCacheTest, CacheUpdate) {
    db.begin_transaction();
    db.set("X", "FirstValue");
    EXPECT_EQ(db.get("X"), "FirstValue");

    db.set("X", "UpdatedValue");
    EXPECT_EQ(db.get("X"), "UpdatedValue"); // Transaction cache should reflect updates during one transaction
    db.commit_transaction();
}

TEST_F(DatabaseCacheTest, TransactionCommit) {
    db.begin_transaction();
    db.set("T", "PendingValue");
    EXPECT_EQ(db.get("T"), "PendingValue");

    db.commit_transaction();
    EXPECT_EQ(db.get("T"), "PendingValue");  // Value should persist
}

TEST_F(DatabaseCacheTest, TransactionRollback) {
    db.begin_transaction();
    db.set("T", "TemporaryValue");
    EXPECT_EQ(db.get("T"), "TemporaryValue");

    db.abort_transaction();
    EXPECT_EQ(db.get("T"), "");  // Value should NOT persist
}

TEST_F(DatabaseCacheTest, ConcurrentAccess) {
    db.begin_transaction();
    std::thread writer([&]() {
        for (int i = 0; i < 10; i++) {
            db.set("Key" + std::to_string(i), "Value" + std::to_string(i));
        }
    });

    std::thread reader([&]() {
        for (int i = 0; i < 10; i++) {
            db.get("Key" + std::to_string(i));
        }
    });

    writer.join();
    reader.join();
    db.commit_transaction();

    EXPECT_EQ(db.get("Key5"), "Value5");  // Verify concurrent access
}

TEST_F(DatabaseCacheTest, RemoveKey) {
    db.begin_transaction();
    db.set("DelKey", "ToDelete");
    db.commit_transaction();
    EXPECT_EQ(db.get("DelKey"), "ToDelete");
    db.begin_transaction();
    db.remove("DelKey");
    db.commit_transaction();
    EXPECT_EQ(db.get("DelKey"), "");  // Should be removed
}

TEST_F(DatabaseCacheTest, RemoveInTransaction) {
    db.begin_transaction();
    db.set("TempKey", "Persisted");
    db.commit_transaction();
    
    db.begin_transaction();
    db.remove("TempKey");
    EXPECT_EQ(db.get("TempKey"), "");  // Inside transaction
    db.abort_transaction();
    
    EXPECT_EQ(db.get("TempKey"), "Persisted");  // Rollback should restore value
}

TEST_F(DatabaseCacheTest, RemoveCommit) {
    db.begin_transaction();
    db.set("RemKey", "SomeValue");
    db.commit_transaction();
    
    db.begin_transaction();
    db.remove("RemKey");
    db.commit_transaction();

    EXPECT_EQ(db.get("RemKey"), "");  // Should be deleted after commit
}
