// Compile: clang++ -std=c++23 -O2 code/dsa/algorithm/binary_search.cpp -o code/build/binary_search
// Run:     ./code/build/binary_search
// LeetCode: standard binary search on sorted array.

#include <iostream>
#include <vector>

int binary_search(const std::vector<int>& nums, int target) {
    int l = 0, r = nums.size() - 1;
    while (l <= r) {
        int m = l + (r - l) / 2;
        if (nums[m] == target) {
            return m;
        }
        if (nums[m] < target) {
            l = m + 1;
        } else {
            r = m - 1;
        }
    }
    return -1;
}

int main() {
    std::vector<int> a{1, 3, 5, 7, 9};
    std::cout << "index of 7 = " << binary_search(a, 7) << "\n";
    return 0;
}
