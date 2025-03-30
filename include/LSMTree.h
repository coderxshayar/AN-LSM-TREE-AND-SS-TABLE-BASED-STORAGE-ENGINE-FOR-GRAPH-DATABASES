#ifndef LSMTREE_H
#define LSMTREE_H

#include <string>
#include <vector>
#include <memory>  // For unique_ptr
#include <mutex>
#include <filesystem>

#include "MemTable.h"
#include "SSTable.h"
#include "Cache.h"

class LSMTree {
public:
    LSMTree(const std::string& base_dir, size_t num_threads = 8);
    void put(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void flush();
    void flush_thread(size_t idx);
    size_t memtable_size() const;
private:
    void load_sstables();
    std::string base_dir_;
    Cache cache_;
    std::vector<MemTable> memtables_;
    std::vector<std::unique_ptr<std::mutex>> memtable_mutexes_;
    std::vector<SSTable> sstables_;
    std::mutex write_mutex_;
    std::mutex log_mutex_;
    size_t memtable_size_limit_;
};

#endif