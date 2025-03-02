

#include "LRUCache.hpp"

LRUCache::LRUCache(int capacity) : mCapacity(capacity) {}

void LRUCache::moveToFront(const std::list<CacheEntry>::iterator& it)
{
    mPriorityList.splice(mPriorityList.begin(), mPriorityList, it);
}

void LRUCache::set(const std::string& key, const std::string& value)
{
    auto entry = mEntries.find(key);
    if (entry!= mEntries.end())
    {
        entry->second->value = value;
        moveToFront(entry->second);
        return;
    }
    if (mPriorityList.size() == mCapacity) 
    {
        removeLastUsed();
    }
    mPriorityList.emplace_front(key, value);
    mEntries[key] = mPriorityList.begin();
}

std::string LRUCache::get(const std::string& key)
{
    auto it = mEntries.find(key);
    if (it == mEntries.end())
    {
        return "";
    }
    moveToFront(it->second);
    return it->second->value;
}

void LRUCache::remove(const std::string& key)
{
    auto it = mEntries.find(key);
    if (it != mEntries.end()) 
    {
        mPriorityList.erase(it->second);
        mEntries.erase(it);
    }
}

void LRUCache::removeLastUsed()
{
    auto it = --mPriorityList.end();
    mEntries.erase(it->key);
    mPriorityList.pop_back();
}   

void LRUCache::clear()
{
    mPriorityList.clear();
    mEntries.clear();
}