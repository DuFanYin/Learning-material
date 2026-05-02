// Build:
//   mkdir -p build
//   clang++ -std=c++20 -O2 code/mini_stl/lru_cache.cpp -o code/build/lru_cache
// Run:
//   ./build/lru_cache

#include <iostream>
#include <string>
#include "lru_cache.h"

int main() {
    LRUCache<std::string, int> cache(2);

    cache.put("a", 1);
    cache.put("b", 2);
    (void)cache.get("a");  // touch a, so b becomes LRU
    cache.put("c", 3);     // evict b

    std::cout << "a=" << cache.get("a").value_or(-1) << "\n";
    std::cout << "b=" << cache.get("b").value_or(-1) << "\n";
    std::cout << "c=" << cache.get("c").value_or(-1) << "\n";
}

