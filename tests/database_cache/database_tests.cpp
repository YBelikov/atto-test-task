#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "i_db.hpp"

class DatabaseInterfaceTest : public ::testing::Test {
protected:
    i_db db{5}; // cache size = 5
};

TEST_F(DatabaseInterfaceTest, BasicSetGet) {
    db.begin_transaction();
    EXPECT_EQ(db.set("A", "Hello"), "OK (Pending transaction)");
    db.commit_transaction();
    EXPECT_EQ(db.get("A"), "Hello");
}

TEST_F(DatabaseInterfaceTest, CacheUpdate) {
    db.begin_transaction();
    db.set("X", "FirstValue");
    EXPECT_EQ(db.get("X"), "FirstValue");

    db.set("X", "UpdatedValue");
    EXPECT_EQ(db.get("X"), "UpdatedValue"); // Transaction cache should reflect updates during one transaction
    db.commit_transaction();
}

TEST_F(DatabaseInterfaceTest, TransactionCommit) {
    db.begin_transaction();
    db.set("T", "PendingValue");
    EXPECT_EQ(db.get("T"), "PendingValue");

    db.commit_transaction();
    EXPECT_EQ(db.get("T"), "PendingValue");  // Value should persist
}

TEST_F(DatabaseInterfaceTest, TransactionRollback) {
    db.begin_transaction();
    db.set("T", "TemporaryValue");
    EXPECT_EQ(db.get("T"), "TemporaryValue");

    db.abort_transaction();
    EXPECT_EQ(db.get("T"), "");  // Value should NOT persist
}

TEST_F(DatabaseInterfaceTest, ConcurrentAccess) {
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

TEST_F(DatabaseInterfaceTest, RemoveKey) {
    db.begin_transaction();
    db.set("DelKey", "ToDelete");
    db.commit_transaction();
    EXPECT_EQ(db.get("DelKey"), "ToDelete");
    db.begin_transaction();
    db.remove("DelKey");
    db.commit_transaction();
    EXPECT_EQ(db.get("DelKey"), "");  // Should be removed
}

TEST_F(DatabaseInterfaceTest, RemoveInTransaction) {
    db.begin_transaction();
    db.set("TempKey", "Persisted");
    db.commit_transaction();
    
    db.begin_transaction();
    db.remove("TempKey");
    EXPECT_EQ(db.get("TempKey"), "");  // Inside transaction
    db.abort_transaction();
    
    EXPECT_EQ(db.get("TempKey"), "Persisted");  // Rollback should restore value
}

TEST_F(DatabaseInterfaceTest, RemoveCommit) {
    db.begin_transaction();
    db.set("RemKey", "SomeValue");
    db.commit_transaction();
    
    db.begin_transaction();
    db.remove("RemKey");
    db.commit_transaction();

    EXPECT_EQ(db.get("RemKey"), "");  // Should be deleted after commit
}

TEST_F(DatabaseInterfaceTest, CacheEviction) {
    db.begin_transaction();
    db.set("1", "One");
    db.set("2", "Two");
    db.set("3", "Three");
    db.set("4", "Four");
    db.set("5", "Five");
    db.set("6", "Six");  // This should evict the least recently used key
    db.commit_transaction();

    EXPECT_EQ(db.get("1"), "One");  // "1" should be evicted
    EXPECT_EQ(db.get("6"), "Six");  // "6" should be in cache
}

TEST_F(DatabaseInterfaceTest, MultipleSetSameKey) {
    db.begin_transaction();
    db.set("A", "Value1");
    db.set("A", "Value2");
    db.set("A", "Value3");
    EXPECT_EQ(db.get("A"), "Value3");  // Should reflect the last write
    db.commit_transaction();

    EXPECT_EQ(db.get("A"), "Value3");  // Ensure final value persists
}

TEST_F(DatabaseInterfaceTest, MultipleRemoveSameKey) {
    db.begin_transaction();
    db.set("B", "ToBeDeleted");
    db.commit_transaction();
    
    db.begin_transaction();
    db.remove("B");
    db.remove("B");
    EXPECT_EQ(db.get("B"), "");  // Should be deleted
    db.commit_transaction();

    EXPECT_EQ(db.get("B"), "");  // Should remain deleted
}

TEST_F(DatabaseInterfaceTest, RemoveBeforeSet) {
    db.begin_transaction();
    db.remove("C");  // Remove before setting
    db.set("C", "NewValue");
    EXPECT_EQ(db.get("C"), "NewValue");  // Should return the newly set value
    db.commit_transaction();

    EXPECT_EQ(db.get("C"), "NewValue");  // Should persist
}

TEST_F(DatabaseInterfaceTest, SetRemoveSameKeyInTransaction) {
    db.begin_transaction();
    db.set("D", "IntermediateValue");
    EXPECT_EQ(db.get("D"), "IntermediateValue");

    db.remove("D");
    EXPECT_EQ(db.get("D"), "");  // Should be removed

    db.set("D", "FinalValue");
    EXPECT_EQ(db.get("D"), "FinalValue");  // Should return last written value
    db.commit_transaction();

    EXPECT_EQ(db.get("D"), "FinalValue");  // Ensure final value persists
}

TEST_F(DatabaseInterfaceTest, RollbackMultipleChanges) {
    db.begin_transaction();
    db.set("E", "ValueE");
    db.set("F", "ValueF");
    db.remove("G");
    db.remove("H");

    EXPECT_EQ(db.get("E"), "ValueE");
    EXPECT_EQ(db.get("F"), "ValueF");
    EXPECT_EQ(db.get("G"), "");
    EXPECT_EQ(db.get("H"), "");

    db.abort_transaction();

    EXPECT_EQ(db.get("E"), "");  // Should NOT exist
    EXPECT_EQ(db.get("F"), "");  // Should NOT exist
    EXPECT_EQ(db.get("G"), "");  // Should NOT exist
    EXPECT_EQ(db.get("H"), "");  // Should NOT exist
}

TEST_F(DatabaseInterfaceTest, ConsecutiveTransactionsModifySameKey) {
    db.begin_transaction();
    db.set("Z", "Transaction1");
    db.commit_transaction();

    db.begin_transaction();
    EXPECT_EQ(db.get("Z"), "Transaction1");  // Should see previous transaction's value
    db.set("Z", "Transaction2");
    db.commit_transaction();

    EXPECT_EQ(db.get("Z"), "Transaction2");  // Should persist the latest update
}
