#include "MemTable.h"

void MemTable::put(const std::string& key, const std::string& value) {
    data_[key] = value;
}

std::string MemTable::get(const std::string& key) const {
    auto it = data_.find(key);
    return (it != data_.end()) ? it->second : "";
}

std::map<std::string, std::string> MemTable::flush() {
    std::map<std::string, std::string> data = data_;
    data_.clear();
    return data;
}