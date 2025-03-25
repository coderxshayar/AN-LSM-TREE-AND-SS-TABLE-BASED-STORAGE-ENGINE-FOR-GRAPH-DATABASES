#include "Cache.h"

Cache::Cache(size_t capacity) : capacity_(capacity) {}

void Cache::put(const std::string& key, const std::string& value) {
    auto it = map_.find(key);
    if (it != map_.end()) {
        lru_list_.erase(it->second);
    } else if (lru_list_.size() >= capacity_) {
        map_.erase(lru_list_.back().first);
        lru_list_.pop_back();
    }
    lru_list_.emplace_front(key, value);
    map_[key] = lru_list_.begin();
}

std::string Cache::get(const std::string& key) {
    auto it = map_.find(key);
    if (it == map_.end()) return "";
    lru_list_.splice(lru_list_.begin(), lru_list_, it->second);
    return it->second->second;
}