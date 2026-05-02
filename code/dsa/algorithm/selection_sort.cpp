// Compile: clang++ -std=c++23 -O2 code/dsa/algorithm/selection_sort.cpp -o code/build/selection_sort
// Run:     ./code/build/selection_sort
// Sorting: selection sort (simple O(n^2) baseline).

#include <iostream>
#include <vector>

void selection_sort(std::vector<int>& a) {
    int n = a.size();
    for (int i = 0; i < n; ++i) {
        int mi = i;
        for (int j = i + 1; j < n; ++j) {
            if (a[j] < a[mi]) mi = j;
        }
        std::swap(a[i], a[mi]);
    }
}

int main() {
    std::vector<int> a{29, 10, 14, 37, 13};
    selection_sort(a);
    for (int x : a) std::cout << x << ' ';
    std::cout << "\n";
    return 0;
}
