#ifndef GRAPHDB_H
#define GRAPHDB_H

#include "LSMTree.h"

class GraphDB {
public:
    GraphDB(const std::string& base_dir);
    void add_edge(const std::string& from, const std::string& to);
    std::string get_neighbors(const std::string& node);
    void flush();
private:
    LSMTree lsm_;
};

#endif