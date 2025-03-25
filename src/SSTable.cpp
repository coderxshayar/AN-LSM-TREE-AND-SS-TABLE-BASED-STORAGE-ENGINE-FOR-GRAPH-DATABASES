#include "SSTable.h"
#include <fstream>

SSTable::SSTable(const std::string& filepath) : filepath_(filepath) {
    load_index(); // Load index on construction
}

void SSTable::load_index() {
    std::ifstream file(filepath_);
    if (!file.is_open()) return; // File might not exist yet
    std::string line;
    std::streampos offset = 0;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            index_[key] = offset;
        }
        offset = file.tellg();
    }
    file.close();
}

void SSTable::write(const std::map<std::string, std::string>& data) {
    std::ofstream file(filepath_);
    std::streampos offset = 0;
    for (const auto& [key, value] : data) {
        file << key << ":" << value << "\n";
        index_[key] = offset;
        offset = file.tellp();
    }
    file.close();
}

std::string SSTable::read(const std::string& key) const {
    auto it = index_.find(key);
    if (it == index_.end()) return "";
    std::ifstream file(filepath_);
    file.seekg(it->second);
    std::string line;
    std::getline(file, line);
    size_t pos = line.find(':');
    return (pos != std::string::npos) ? line.substr(pos + 1) : "";
}