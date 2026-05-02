// Build:
//   mkdir -p code/build
//   clang++ -std=c++20 -O2 code/mini_stl/vector.cpp -o code/build/vector
// Run:
//   ./code/build/vector

#include <iostream>
#include "vector.h"

int main() {
    Vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.emplace_back(30);

    std::cout << "size=" << v.size() << " capacity=" << v.capacity() << "\n";
    for (auto x : v) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    v.pop_back();
    std::cout << "after pop_back: size=" << v.size() << " last=" << v[v.size() - 1] << "\n";
}

