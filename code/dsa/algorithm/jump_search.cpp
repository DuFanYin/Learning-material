// Compile: clang++ -std=c++23 -O2 code/dsa/algorithm/jump_search.cpp -o code/build/jump_search
// Run:     ./code/build/jump_search
// Basic: jump search on sorted array.

#include <cmath>
#include <iostream>
#include <vector>

int jump_search(const std::vector<int>& a, int target) {
    int n = a.size();
    if (!n) return -1;
    int step = std::sqrt(n);
    int prev = 0;
    int cur = step;

    while (prev < n && a[std::min(cur, n) - 1] < target) {
        prev = cur;
        cur += step;
        if (prev >= n) return -1;
    }
    for (int i = prev; i < std::min(cur, n); ++i) {
        if (a[i] == target) return i;
    }
    return -1;
}

int main() {
    std::vector<int> a{1, 3, 5, 7, 9, 11, 13, 15, 17};
    std::cout << "index(11) = " << jump_search(a, 11) << "\n";
    std::cout << "index(10) = " << jump_search(a, 10) << "\n";
    return 0;
}
