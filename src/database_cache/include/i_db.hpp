#ifndef I_DB 
#define I_DB

#include <string>
#include <unordered_set>
#include <shared_mutex>
#include "LRUCache.hpp"

struct TransactionState {
    std::unordered_map<std::string, std::string> pendingWrites;
    std::unordered_set<std::string> pendingDeletes;
    bool active = false;
    void cleanTransactionData() 
    {
        active = false;
        pendingWrites.clear();
        pendingDeletes.clear();
    }
};

struct i_db
{
public:
    explicit i_db(int cacheSize);
    i_db() = default;
    bool begin_transaction();
    bool commit_transaction();
    bool abort_transaction();
    std::string get(const std::string& key);
    std::string set(const std::string& key, const std::string& data);
    std::string remove(const std::string& key);

private:
    TransactionState mTransactionState;
    std::shared_mutex mTransactionMutex; // I am trying to do reader-writer lock (concept I am familiar from Swift)
    std::unique_lock<std::shared_mutex> mWriterLock;
    std::shared_lock<std::shared_mutex> mReaderLock;
    LRUCache mCache;
    std::unordered_map<std::string, std::string> mStorage; // Dummy backend since we don't have any "real" database here
};

#endif