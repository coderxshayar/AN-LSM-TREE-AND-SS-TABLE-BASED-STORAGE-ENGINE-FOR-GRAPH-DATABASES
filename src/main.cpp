#include "GraphDB.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <filesystem>

void insert_batch(GraphDB& db, int start, int end) {
    for (int i = start; i < end; i++) {
        std::string from = "node" + std::to_string(i % 10000);
        std::string to = "node" + std::to_string((i + 1) % 10000);
        db.add_edge(from, to);
    }
}
int main() {
    std::filesystem::remove_all("./data/sst");
    GraphDB db("./data");
    const int edges = 50000;
    const int threads = 8;
    const int batch_size = edges / threads;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> thread_pool;
    for (int i = 0; i < threads; i++) {
        int batch_start = i * batch_size;
        int batch_end = (i == threads - 1) ? edges : (i + 1) * batch_size;
        thread_pool.emplace_back(insert_batch, std::ref(db), batch_start, batch_end);
    }
    for (auto& t : thread_pool) t.join();
    auto insert_end = std::chrono::high_resolution_clock::now();
    db.flush();
    auto total_end = std::chrono::high_resolution_clock::now();

    auto insert_ms = std::chrono::duration_cast<std::chrono::milliseconds>(insert_end - start).count();
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - start).count();
    std::cout << "Insert time: " << insert_ms << " ms (" << (edges * 1000.0 / insert_ms) << " edges/sec)\n";
    std::cout << "Total time (with flush): " << total_ms << " ms (" << (edges * 1000.0 / total_ms) << " edges/sec)\n";

    std::cout << "Neighbors of node0: " << db.get_neighbors("node0") << "\n";
    std::cout << "Neighbors of node1: " << db.get_neighbors("node1") << "\n";
    std::cout << "Neighbors of node9999: " << db.get_neighbors("node9999") << "\n";
    std::cout << "Neighbors of node10000: " << db.get_neighbors("node10000") << "\n";

    return 0;
}

