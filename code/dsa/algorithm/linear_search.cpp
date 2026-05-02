// Compile: clang++ -std=c++23 -O2 code/dsa/algorithm/linear_search.cpp -o code/build/linear_search
// Run:     ./code/build/linear_search
// LeetCode/basic: linear scan search for unsorted arrays.

#include <iostream>
#include <vector>

int linear_search(const std::vector<int>& a, int target) {
    for (int i = 0; i < a.size(); ++i) {
        if (a[i] == target) return i;
    }
    return -1;
}

int main() {
    std::vector<int> a{9, 1, 5, 3, 7};
    std::cout << "index(5) = " << linear_search(a, 5) << "\n";
    std::cout << "index(8) = " << linear_search(a, 8) << "\n";
    return 0;
}
