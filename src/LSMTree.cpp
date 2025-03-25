#include "LSMTree.h"
#include <filesystem>
#include <thread>
#include <fstream>

LSMTree::LSMTree(const std::string& base_dir) 
    : base_dir_(base_dir), wal_(base_dir + "/wal/wal.log"), cache_(1000) {
    sstables_.reserve(10);
    std::filesystem::create_directories(base_dir + "/wal");
    std::filesystem::create_directories(base_dir + "/sst");
    load_sstables(); // Load existing SSTables
}

void LSMTree::load_sstables() {
    for (const auto& entry : std::filesystem::directory_iterator(base_dir_ + "/sst")) {
        if (entry.path().extension() == ".sst") {
            sstables_.emplace_back(entry.path().string());
        }
    }
}

void LSMTree::put(const std::string& key, const std::string& value) {
    wal_.append(key, value);
    memtable_.put(key, value);
    if (memtable_.size() >= memtable_size_limit_) {
        auto current_data = memtable_.flush();
        SSTable sst(base_dir_ + "/sst/sst_" + std::to_string(sstables_.size()) + ".sst");
        sst.write(current_data);
        sstables_.push_back(sst);
        wal_.clear();
        if (sstables_.size() > 5 && !compacting_) {
            start_background_compaction();
        }
    }
}

std::string LSMTree::get(const std::string& key) {
    std::string value = memtable_.get(key);
    if (!value.empty()) return value;
    if (!(value = cache_.get(key)).empty()) return value;
    for (const auto& sst : sstables_) {
        value = sst.read(key);
        if (!value.empty()) {
            cache_.put(key, value);
            return value;
        }
    }
    return "";
}

void LSMTree::compact() {
    if (sstables_.size() <= 1) return;
    std::map<std::string, std::string> merged;
    for (const auto& sst : sstables_) {
        std::ifstream file(sst.get_filepath());
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                merged[line.substr(0, pos)] = line.substr(pos + 1);
            }
        }
        file.close();
        std::filesystem::remove(sst.get_filepath());
    }
    sstables_.clear();
    SSTable new_sst(base_dir_ + "/sst/sst_0.sst");
    new_sst.write(merged);
    sstables_.push_back(new_sst);
}

void LSMTree::start_background_compaction() {
    if (compacting_.exchange(true)) return;
    std::thread([this]() {
        compact();
        compacting_ = false;
    }).detach();
}

void LSMTree::flush() {
    if (!memtable_.empty()) {
        auto current_data = memtable_.flush();
        SSTable sst(base_dir_ + "/sst/sst_" + std::to_string(sstables_.size()) + ".sst");
        sst.write(current_data);
        sstables_.push_back(sst);
        wal_.clear();
    }
}