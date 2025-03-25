#ifndef WAL_H
#define WAL_H

#include <string>
#include <fstream>
#include <vector>

class WAL {
public:
    WAL(const std::string& filepath);
    ~WAL();
    void append(const std::string& key, const std::string& value);
    void clear();
private:
    std::ofstream file_;
    std::string filepath_;
    std::vector<std::string> buffer_;
    static constexpr size_t BATCH_SIZE = 100;
};

#endif