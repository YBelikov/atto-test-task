#include "i_db.hpp"

namespace {
    const std::string okStatusTransactionDesc = "OK (Pending transaction)";
    const std::string failedStatusTransactionDesc = "Failed! No pending transactions!";
}

i_db::i_db(int cacheSize)
{
    mCache = LRUCache(cacheSize);
}

bool i_db::begin_transaction() 
{
    std::unique_lock lock(mTransactionMutex);
    if (mTransactionState.active) { return false; } 
    mTransactionState.active = true;
    return true;
}

bool i_db::commit_transaction() 
{
    std::unique_lock lock(mTransactionMutex);
    if (!mTransactionState.active) { return false; }
    for (const auto& erasedData : mTransactionState.pendingDeletes)
    {
        mCache.remove(erasedData);
        mStorage.erase(erasedData);
    }
    for (const auto& [key, value] : mTransactionState.pendingWrites) {
        if (mTransactionState.pendingDeletes.find(key) == mTransactionState.pendingDeletes.end()) {
            mCache.set(key, value);
            mStorage[key] = value;
        }
    }
    mTransactionState.cleanTransactionData();
    return true;
}

bool i_db::abort_transaction() 
{
    std::unique_lock lock(mTransactionMutex);
    if (!mTransactionState.active) { return false; } 
    mTransactionState.cleanTransactionData();
    return true;
}

std::string i_db::set(const std::string& key, const std::string& data)
{   
    std::unique_lock lock(mTransactionMutex);
    if (!mTransactionState.active) { return failedStatusTransactionDesc; }
    mTransactionState.pendingWrites[key] = data;
    mTransactionState.pendingDeletes.erase(key); // undo removal
    return okStatusTransactionDesc;
}

std::string i_db::get(const std::string& key)
{
    std::shared_lock readLock(mTransactionMutex);
    if (mTransactionState.pendingDeletes.find(key) != mTransactionState.pendingDeletes.end())
    {
        return "";
    }
    if (mTransactionState.pendingWrites.find(key) != mTransactionState.pendingWrites.end())
    {
        return mTransactionState.pendingWrites[key];
    }
    auto data = mCache.get(key); // Check main cache first
    if (!data.empty()) { return data; }
    if (mStorage.find(key) == mStorage.end()) { return ""; }
    data = mStorage[key];
    mCache.set(key, data); // Cache the accessed data
    return data;
}

std::string i_db::remove(const std::string& key) 
{   
    std::unique_lock lock(mTransactionMutex);
    if (!mTransactionState.active) 
    {
        return failedStatusTransactionDesc;
    }
    if (mTransactionState.pendingWrites.find(key) != mTransactionState.pendingWrites.end())
    {
        mTransactionState.pendingWrites.erase(key);
        return okStatusTransactionDesc;
    }
    mTransactionState.pendingDeletes.insert(key);
    return okStatusTransactionDesc;
}