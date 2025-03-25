#ifndef CACHE_H
#define CACHE_H

#include <unordered_map>
#include <list>
#include <string>

class Cache {
public:
    Cache(size_t capacity);
    void put(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
private:
    size_t capacity_;
    std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator> map_;
    std::list<std::pair<std::string, std::string>> lru_list_;
};

#endif