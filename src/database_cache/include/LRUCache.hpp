#ifndef LRUCACHE
#define LRUCACHE

#include <string>
#include <map>
#include <list>

class LRUCache 
{
public:
    LRUCache() = default;
    explicit LRUCache(int capacity);
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void remove(const std::string& key);
    void invalidate();

private:
    int mCapacity;
    std::unordered_map<std::string, std::string> cache;
};

#endif