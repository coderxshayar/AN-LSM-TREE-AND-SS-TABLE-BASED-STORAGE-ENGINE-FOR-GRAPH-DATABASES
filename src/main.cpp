#include "GraphDB.h"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    // Step 1: Insert data and flush to SSTables
    {
        GraphDB db("./data");
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 15000; i++) {
            std::string from = "node" + std::to_string(i % 10000);
            std::string to = "node" + std::to_string((i + 1) % 10000);
            db.add_edge(from, to);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Insert time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
        
        // Flush all data to SSTables
        // db.flush();
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for any background compaction
    } // db is destroyed here, ensuring no in-memory data remains

    // Step 2: Restart and query from SSTables only
    {
        GraphDB db("./data"); // New instance, memtable and cache are empty

        // Queries
        std::cout << "Neighbors of node100: " << db.get_neighbors("node100") << "\n";
        std::cout << "Neighbors of node1929: " << db.get_neighbors("node1929") << "\n";
        std::cout << "Neighbors of node9999: " << db.get_neighbors("node9999") << "\n";
        
        // Check a non-existent node
        std::cout << "Neighbors of node10000: " << db.get_neighbors("node10000") << "\n";
    }

    {
    GraphDB db("./data"); // New instance, memtable and cache are empty

    auto start = std::chrono::high_resolution_clock::now();
    
    // Queries
    std::cout << "Neighbors of node100: " << db.get_neighbors("node100") << "\n";
    std::cout << "Neighbors of node1929: " << db.get_neighbors("node1929") << "\n";
    std::cout << "Neighbors of node9999: " << db.get_neighbors("node9999") << "\n";
    
    // Check a non-existent node
    std::cout << "Neighbors of node10000: " << db.get_neighbors("node10000") << "\n";
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "All queries took: " << elapsed.count() << " seconds\n";
}
    return 0;
}