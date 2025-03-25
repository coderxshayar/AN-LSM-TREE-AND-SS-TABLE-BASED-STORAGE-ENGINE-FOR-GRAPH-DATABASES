#include "GraphDB.h"
#include <sstream>
#include <set>

GraphDB::GraphDB(const std::string& base_dir) : lsm_(base_dir) {}

void GraphDB::add_edge(const std::string& from, const std::string& to) {
    std::string neighbors = lsm_.get(from);
    std::set<std::string> neighbor_set;
    
    // Parse existing neighbors into a set
    if (!neighbors.empty()) {
        std::stringstream ss(neighbors);
        std::string neighbor;
        while (std::getline(ss, neighbor, ',')) {
            neighbor_set.insert(neighbor);
        }
    }
    
    // Add new neighbor
    neighbor_set.insert(to);
    
    // Rebuild the neighbors string
    std::string new_neighbors;
    for (auto it = neighbor_set.begin(); it != neighbor_set.end(); ++it) {
        if (!new_neighbors.empty()) new_neighbors += ",";
        new_neighbors += *it;
    }
    
    lsm_.put(from, new_neighbors);
}
std::string GraphDB::get_neighbors(const std::string& node) {
    return lsm_.get(node);
}

void GraphDB::flush() {
    lsm_.flush();
}