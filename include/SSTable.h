#ifndef SSTABLE_H
#define SSTABLE_H

#include <string>
#include <map>

class SSTable {
public:
    SSTable(const std::string& filepath);
    void write(const std::map<std::string, std::string>& data);
    std::string read(const std::string& key) const;
    std::string get_filepath() const { return filepath_; } // Add this method
private:
    void load_index();
    std::string filepath_;
    std::map<std::string, std::streampos> index_; // Key to file offset
};

#endif