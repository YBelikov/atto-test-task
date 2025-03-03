#ifndef LRUCACHE
#define LRUCACHE

#include <string>
#include <unordered_map>
#include <list>

struct CacheEntry 
{
    std::string key;
    std::string value;

    CacheEntry(const std::string& key, const std::string& value) 
    {
        this->key = key;
        this->value = value;
    }
};

class LRUCache 
{
public:
    LRUCache() = default;
    explicit LRUCache(int capacity);
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void remove(const std::string& key);
    void clear();

private:
    void moveToFront(const std::list<CacheEntry>::iterator& it);
    void removeLastUsed();

    int mCapacity;
    std::unordered_map<std::string, std::list<CacheEntry>::iterator> mEntries;
    std::list<CacheEntry> mPriorityList;
};

#endif