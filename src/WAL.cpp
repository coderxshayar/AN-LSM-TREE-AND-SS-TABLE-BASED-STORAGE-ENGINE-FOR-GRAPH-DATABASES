// #include "WAL.h"
// #include <iostream>

// WAL::WAL(const std::string& filepath) : filepath_(filepath) {
//     std::cout << "Opening WAL file: " << filepath_ << "\n";
//     file_.open(filepath_, std::ios::app);
//     if (!file_.is_open()) {
//         std::cerr << "Failed to open WAL file: " << filepath_ << "\n";
//     }
// }

// WAL::~WAL() {
//     std::cout << "Closing WAL\n";
//     if (file_.is_open()) {
//         flush();
//         file_.close();
//     }
// }

// void WAL::append(const std::string& key, const std::string& value) {
//     std::cout << "Attempting to append: " << key << " -> " << value << "\n";
//     bool should_flush = false;
//     {
//         std::lock_guard<std::mutex> lock(mutex_);
//         if (keys_.count(key) == 0) {
//             std::string entry = key + ":" + value + "\n";
//             buffer_.push_back(entry);
//             keys_.insert(key);
//             std::cout << "Appended, Buffer size: " << buffer_.size() << "\n";
//             if (buffer_.size() >= BATCH_SIZE) {
//                 should_flush = true;
//             }
//         } else {
//             std::cout << "Duplicate key skipped: " << key << "\n";
//         }
//     }
//     if (should_flush) {
//         flush();
//     }
// }

// void WAL::flush() {
//     std::cout << "Flushing WAL\n";
//     std::lock_guard<std::mutex> lock(mutex_);
//     if (file_.is_open()) {
//         for (const auto& entry : buffer_) {
//             file_ << entry; // Batch write
//         }
//         file_.flush();
//         std::cout << "WAL flushed\n";
//     }
// }

// void WAL::clear() {
//     std::cout << "Clearing WAL\n";
//     std::lock_guard<std::mutex> lock(mutex_);
//     buffer_.clear();
//     keys_.clear();
//     if (file_.is_open()) {
//         file_.close();
//     }
//     file_.open(filepath_, std::ios::trunc);
//     if (!file_.is_open()) {
//         std::cerr << "Failed to reopen WAL file: " << filepath_ << "\n";
//     }
// }

// std::vector<std::string> WAL::recover() {
//     std::cout << "Starting WAL recovery\n";
//     std::lock_guard<std::mutex> lock(mutex_);
//     std::vector<std::string> entries;
//     std::ifstream in_file(filepath_);
//     if (!in_file.is_open()) {
//         std::cout << "No WAL file to recover: " << filepath_ << "\n";
//         return entries;
//     }
//     std::string line;
//     while (std::getline(in_file, line)) {
//         if (!line.empty() && keys_.count(line.substr(0, line.find(':'))) == 0) {
//             entries.push_back(line);
//             keys_.insert(line.substr(0, line.find(':')));
//         }
//     }
//     in_file.close();
//     std::cout << "Recovered " << entries.size() << " entries from WAL\n";
//     return entries;
// }


#include "WAL.h"
#include <iostream>

WAL::WAL(const std::string& filepath) : filepath_(filepath) {
    file_.open(filepath_, std::ios::app);
    if (!file_.is_open()) {
        std::cerr << "Failed to open WAL file: " << filepath_ << "\n";
    }
}

WAL::~WAL() {
    if (file_.is_open()) {
        flush();
        file_.close();
    }
}

void WAL::append(const std::string& key, const std::string& value) {
    std::string entry = key + ":" + value + "\n";
    buffer_.push_back(std::move(entry)); // Move to avoid copy
}

void WAL::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_.is_open() && !buffer_.empty()) {
        for (const auto& entry : buffer_) {
            file_ << entry;
        }
        file_.flush();
    }
}

void WAL::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.clear();
    if (file_.is_open()) {
        file_.close();
    }
    file_.open(filepath_, std::ios::trunc);
    if (!file_.is_open()) {
        std::cerr << "Failed to reopen WAL file: " << filepath_ << "\n";
    }
}

std::vector<std::string> WAL::recover() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> entries;
    std::ifstream in_file(filepath_);
    if (!in_file.is_open()) {
        return entries;
    }
    std::string line;
    while (std::getline(in_file, line)) {
        if (!line.empty()) {
            entries.push_back(line);
        }
    }
    in_file.close();
    return entries;
}