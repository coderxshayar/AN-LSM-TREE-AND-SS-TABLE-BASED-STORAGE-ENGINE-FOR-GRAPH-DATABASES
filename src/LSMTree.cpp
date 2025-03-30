#include "LSMTree.h"
#include <iostream>
#include <filesystem>
LSMTree::LSMTree(const std::string& base_dir, size_t num_threads) 
    : base_dir_(base_dir), cache_(1000), memtable_size_limit_(15000) {
    memtables_.resize(num_threads);
    memtable_mutexes_.resize(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
        memtable_mutexes_[i] = std::make_unique<std::mutex>();
    }
    std::filesystem::create_directories(base_dir_ + "/sst");
    load_sstables();
    std::cout << "Initialized with " << sstables_.size() << " SSTables\n";
}


void LSMTree::put(const std::string& key, const std::string& value) {
    size_t thread_idx = std::hash<std::thread::id>{}(std::this_thread::get_id()) % memtables_.size();
    std::lock_guard<std::mutex> lock(*memtable_mutexes_[thread_idx]);
    memtables_[thread_idx].put(key, value);
    // No flush here—let it grow
}


std::string LSMTree::get(const std::string& key) {
    for (size_t i = 0; i < memtables_.size(); ++i) {
        std::lock_guard<std::mutex> lock(*memtable_mutexes_[i]);
        std::string value = memtables_[i].get(key);
        if (!value.empty()) return value;
    }
    std:: string value = cache_.get(key);
    if (!value.empty()) return value;
    for (const auto& sst : sstables_) {
        std::string value = sst.read(key);
        if (!value.empty()) {
            cache_.put(key, value);
            return value;
        }
    }
    return "";
}

void LSMTree::flush() {
    for (size_t i = 0; i < memtables_.size(); ++i) {
        std::lock_guard<std::mutex> lock(*memtable_mutexes_[i]);  // Lock each memtable
        if (memtables_[i].size() > 0) {
            std::lock_guard<std::mutex> log_lock(log_mutex_);
            std::cout << "Flushing memtable " << i << " with " << memtables_[i].size() << " entries\n";
            auto current_data = memtables_[i].flush();
            std::string filepath = base_dir_ + "/sst/sst_" + std::to_string(sstables_.size()) + ".sst";
            SSTable sst(filepath);
            sst.write(current_data);
            sstables_.push_back(std::move(sst));
            std::cout << "SSTable count: " << sstables_.size() << "\n";
        }
    }
}

void LSMTree::flush_thread(size_t idx) {
    std::lock_guard<std::mutex> lock(write_mutex_);
    if (memtables_[idx].size() > 0) {
        std::lock_guard<std::mutex> log_lock(log_mutex_);
        std::cout << "Flushing memtable " << idx << " with " << memtables_[idx].size() << " entries\n";
        auto current_data = memtables_[idx].flush();
        std::string filepath = base_dir_ + "/sst/sst_" + std::to_string(sstables_.size()) + ".sst";
        SSTable sst(filepath);
        sst.write(current_data);
        sstables_.push_back(std::move(sst));
        std::cout << "SSTable count: " << sstables_.size() << "\n";
    }
}

size_t LSMTree::memtable_size() const {
    size_t total = 0;
    for (const auto& mt : memtables_) {
        total += mt.size();
    }
    return total;
}

void LSMTree::load_sstables() {
    for (const auto& entry : std::filesystem::directory_iterator(base_dir_ + "/sst")) {
        if (entry.path().extension() == ".sst") {
            sstables_.emplace_back(entry.path().string());
        }
    }
}