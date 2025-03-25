#ifndef MEMTABLE_H
#define MEMTABLE_H

#include <map>
#include <string>

class MemTable {
public:
    void put(const std::string& key, const std::string& value);
    std::string get(const std::string& key) const;
    bool empty() const { return data_.empty(); }
    std::map<std::string, std::string> flush(); // Flush to SSTable
    size_t size() const { return data_.size(); }
private:
    std::map<std::string, std::string> data_;
};

#endif