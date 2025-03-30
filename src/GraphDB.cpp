#include "GraphDB.h"
#include <sstream>
#include <set>
#include <iostream>

void GraphDB::add_edge(const std::string& from, const std::string& to) {
    size_t shard = std::hash<std::string>{}(from) % 64;
    std::lock_guard<std::mutex> lock(add_edge_mutex_[shard]);
    std::string neighbors = lsm_.get(from);
    if (neighbors.empty()) {
        lsm_.put(from, to);
    } else if (neighbors.find(to) == std::string::npos) {
        std::string new_neighbors;
        new_neighbors.reserve(neighbors.size() + to.size() + 1);
        new_neighbors = neighbors + "," + to;
        lsm_.put(from, new_neighbors);
    }
}
std::string GraphDB::get_neighbors(const std::string& from) {
    return lsm_.get(from);
}

void GraphDB::flush() {
    lsm_.flush();
}