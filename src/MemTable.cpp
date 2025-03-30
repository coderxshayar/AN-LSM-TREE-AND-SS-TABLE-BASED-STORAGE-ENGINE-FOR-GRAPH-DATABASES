#include "MemTable.h"
#include <iostream>

void MemTable::put(const std::string& key, const std::string& value) {
    tbb::concurrent_hash_map<std::string, std::string>::accessor acc;
    if (data_.insert(acc, key)) {
        acc->second = value;
    } else {
        acc->second = value;
    }
}
// void MemTable::put(const std::string& key, const std::string& value) {
//     std::lock_guard<std::mutex> lock(mutex_);
//     tbb::concurrent_hash_map<std::string, std::string>::accessor acc;
//     std::cout << "Inserting into memtable, key: " << key << "\n";
//     if (data_.insert(acc, key)) {
//         std::cout << "New key inserted: " << key << "\n";
//         acc->second = value;
//     } else {
//         std::cout << "Updating existing key: " << key << "\n";
//         acc->second = value;
//     }
// }
std::string MemTable::get(const std::string& key) const {
    tbb::concurrent_hash_map<std::string, std::string>::const_accessor acc;
    if (data_.find(acc, key)) {
        return acc->second;
    }
    return "";
}

std::map<std::string, std::string> MemTable::flush() {
    std::map<std::string, std::string> result;
    for (const auto& item : data_) {
        result[item.first] = item.second;
    }
    // std::cout << "Before clear: " << data_.size() << "\n";
    data_.clear();
    // std::cout << "After clear: " << data_.size() << "\n";
    return result;
}