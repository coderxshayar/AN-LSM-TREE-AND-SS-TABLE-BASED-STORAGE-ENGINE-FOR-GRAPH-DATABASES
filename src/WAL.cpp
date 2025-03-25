#include "WAL.h"

WAL::WAL(const std::string& filepath) : filepath_(filepath) {
    file_.open(filepath, std::ios::app);
}

WAL::~WAL() {
    if (file_.is_open()) file_.close();
}

void WAL::append(const std::string& key, const std::string& value) {
    if (file_.is_open()) {
        file_ << key << ":" << value << "\n";
        file_.flush(); // Ensure durability
    }
}

void WAL::clear() {
    file_.close();
    file_.open(filepath_, std::ios::trunc); // Clear file
}