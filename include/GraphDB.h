#ifndef GRAPHDB_H
#define GRAPHDB_H

#include "LSMTree.h"
#include <string>
#include <mutex>

class GraphDB {
public:
    // GraphDB(const std::string& base_dir);
    GraphDB(const std::string& base_dir) : lsm_(base_dir, 8) {}
    void add_edge(const std::string& from, const std::string& to);
    std::string get_neighbors(const std::string& from);
    void flush();
private:
    LSMTree lsm_;
    std::array<std::mutex, 64> add_edge_mutex_;// Protect read-modify-write cycle
};

#endif