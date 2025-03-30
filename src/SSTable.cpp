#include "SSTable.h"
#include <fstream>
#include <vector>
#include <iostream>

SSTable::SSTable(const std::string& filepath) : filepath_(filepath) {
    bloom_parameters params;
    params.projected_element_count = 10000; // Max keys per SSTable
    params.false_positive_probability = 0.01; // 1% false positive rate
    params.random_seed = 0xA5A5A5A5; // Consistent seed
    params.compute_optimal_parameters();
    bf_ = bloom_filter(params);
    load_index();
}

void SSTable::load_index() {
    std::ifstream file(filepath_, std::ios::binary);
    if (!file.is_open()) return;
    std::streampos offset = 0;
    while (file.good()) {
        uLongf compressed_size;
        file.read(reinterpret_cast<char*>(&compressed_size), sizeof(compressed_size));
        if (!file.good()) break;
        std::vector<unsigned char> compressed(compressed_size);
        file.read(reinterpret_cast<char*>(compressed.data()), compressed_size);
        uLongf uncompressed_size = 1024;
        std::vector<unsigned char> uncompressed(uncompressed_size);
        uncompress(uncompressed.data(), &uncompressed_size, compressed.data(), compressed_size);
        std::string line(uncompressed.begin(), uncompressed.begin() + uncompressed_size);
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            index_[key] = {offset, compressed_size};
            bf_.insert(key);
        }
        offset = file.tellg();
    }
    file.close();
}
void SSTable::write(const std::map<std::string, std::string>& data) {
    std::ofstream file(filepath_, std::ios::binary);
    for (const auto& [key, value] : data) {
        std::string line = key + ":" + value + "\n";
        uLongf uncompressed_size = line.size();
        uLongf compressed_size = compressBound(uncompressed_size);
        std::vector<unsigned char> compressed(compressed_size);
        compress(compressed.data(), &compressed_size, (Bytef*)line.data(), uncompressed_size);
        file.write(reinterpret_cast<char*>(&compressed_size), sizeof(compressed_size));
        file.write(reinterpret_cast<char*>(compressed.data()), compressed_size);
    }
}
std::string SSTable::read(const std::string& key) const {
    std::ifstream file(filepath_, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open SSTable: " << filepath_ << "\n";
        return "";
    }
    while (file.good()) {
        uLongf compressed_size;
        file.read(reinterpret_cast<char*>(&compressed_size), sizeof(compressed_size));
        if (!file.good()) break;
        if (compressed_size > 1024 * 1024) { // Sanity check: max 1MB
            std::cerr << "Invalid compressed_size: " << compressed_size << "\n";
            break;
        }
        std::vector<unsigned char> compressed(compressed_size);
        file.read(reinterpret_cast<char*>(compressed.data()), compressed_size);
        if (!file.good()) {
            std::cerr << "Failed to read compressed data\n";
            break;
        }
        std::vector<unsigned char> decompressed(compressed_size * 2); // Oversized
        uLongf dest_len = decompressed.size();
        int result = uncompress(decompressed.data(), &dest_len, compressed.data(), compressed_size);
        if (result != Z_OK) {
            std::cerr << "Decompression failed: " << result << "\n";
            break;
        }
        std::string line(reinterpret_cast<char*>(decompressed.data()), dest_len);
        size_t pos = line.find(':');
        if (pos != std::string::npos && line.substr(0, pos) == key) {
            std::string value = line.substr(pos + 1, line.size() - pos - 2); // Trim '\n'
            std::cout << "SSTable read: " << key << " -> " << value << "\n";
            return value;
        }
    }
    std::cout << "No value for " << key << " in " << filepath_ << "\n";
    return "";
}

std::map<std::string, std::string> SSTable::read_all() const {
    std::map<std::string, std::string> data;
    std::ifstream file(filepath_, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open SSTable for read_all: " << filepath_ << "\n";
        return data;
    }
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::streampos offset = 0;
    while (offset < file_size) {
        uLongf compressed_size;
        file.read(reinterpret_cast<char*>(&compressed_size), sizeof(compressed_size));
        std::vector<unsigned char> compressed(compressed_size);
        file.read(reinterpret_cast<char*>(compressed.data()), compressed_size);
        uLongf uncompressed_size = compressed_size * 2;
        std::vector<unsigned char> uncompressed(uncompressed_size);
        int result = uncompress(uncompressed.data(), &uncompressed_size, compressed.data(), compressed_size);
        while (result == Z_BUF_ERROR) {
            uncompressed_size *= 2;
            uncompressed.resize(uncompressed_size);
            result = uncompress(uncompressed.data(), &uncompressed_size, compressed.data(), compressed_size);
        }
        if (result != Z_OK) {
            std::cerr << "Decompression failed in read_all\n";
            break;
        }
        std::string line(uncompressed.begin(), uncompressed.begin() + uncompressed_size);
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            data[key] = value;
        }
        offset = file.tellg();
    }
    return data;
}