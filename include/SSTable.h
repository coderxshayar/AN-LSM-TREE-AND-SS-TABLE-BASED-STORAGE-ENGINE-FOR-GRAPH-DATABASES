#ifndef SSTABLE_H
#define SSTABLE_H

#include <string>
#include <map>
#include <zlib.h>
#include "bloom_filter.hpp" // Use standalone header

class SSTable {
public:
    SSTable(const std::string& filepath);
    void write(const std::map<std::string, std::string>& data);
    std::string read(const std::string& key) const;
    std::string get_filepath() const { return filepath_; }
    std::map<std::string, std::string> read_all() const;
private:
    void load_index();
    std::string filepath_;
    mutable std::map<std::string, std::pair<std::streampos, uLongf>> index_; // Offset + compressed size
    bloom_filter bf_; // Standalone Bloom filter
};

#endif