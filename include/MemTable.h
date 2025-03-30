#ifndef MEMTABLE_H
#define MEMTABLE_H

#include <tbb/concurrent_hash_map.h>
#include <string>
#include <map>
#include <mutex>
class MemTable {
public:
    MemTable()= default;
    void put(const std::string& key, const std::string& value);
    std::string get(const std::string& key) const;
    bool empty() const { return data_.empty(); }
    std::map<std::string, std::string> flush(); // Convert to std::map for SSTable compatibility
    size_t size() const { return data_.size(); }
private:
    tbb::concurrent_hash_map<std::string, std::string> data_;
    size_t memtable_size_limit_;
    // std::mutex mutex_        ;              
};

#endif