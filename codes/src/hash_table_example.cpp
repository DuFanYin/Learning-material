// Build:
//   mkdir -p build
//   clang++ -std=c++20 -O2 codes/src/hash_table_example.cpp -o build/hash_table_example
// Run:
//   ./build/hash_table_example

#include <iostream>
#include <string>
#include "../includes/hash_table.h"

int main() {
    HashTable<std::string, int> ht;

    ht.insert_or_assign("alice", 1);
    ht.insert_or_assign("bob", 2);
    ht.insert_or_assign("alice", 3);  // update

    if (auto* v = ht.find("alice")) {
        std::cout << "alice=" << *v << "\n";
    }

    ht.erase("bob");
    std::cout << "size=" << ht.size() << "\n";
}

