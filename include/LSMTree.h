#ifndef LSMTREE_H
#define LSMTREE_H

#include "MemTable.h"
#include "WAL.h"
#include "SSTable.h"
#include "Cache.h"
#include <vector>
#include <atomic>

class LSMTree {
public:
    LSMTree(const std::string& base_dir);
    void put(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void compact();
    void start_background_compaction();
    void flush();
private:
    void load_sstables(); // Add this
    MemTable memtable_;
    WAL wal_;
    std::vector<SSTable> sstables_;
    Cache cache_;
    std::string base_dir_;
    size_t memtable_size_limit_ = 10000;
    std::atomic<bool> compacting_{false};
};

#endif