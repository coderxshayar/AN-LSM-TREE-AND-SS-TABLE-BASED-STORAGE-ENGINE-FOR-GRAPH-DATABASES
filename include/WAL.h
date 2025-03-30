#ifndef WAL_H
#define WAL_H

#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <set>
class WAL {
public:
    WAL(const std::string& filepath);
    ~WAL();
    void append(const std::string& key, const std::string& value);
    void clear();
    void flush();
    std::vector<std::string> recover(); // Added for log recovery
private:
    std::ofstream file_;
    std::string filepath_;
    std::vector<std::string> buffer_;
    static constexpr size_t BATCH_SIZE = 10000;
    std::mutex mutex_;
    std::set<std::string> keys_;
};

#endif