

#include "LRUCache.hpp"

LRUCache::LRUCache(int capacity) : mCapacity(capacity) {}

void LRUCache::set(const std::string& key, const std::string& value)
{
    cache[key] = value;
}

std::string LRUCache::get(const std::string& key)
{
    auto it = cache.find(key);
    if (it == cache.end()) return "";
    return it->second;
}

void LRUCache::remove(const std::string& key)
{
    cache.erase(key);
}

void LRUCache::invalidate()
{
    cache.clear();
}