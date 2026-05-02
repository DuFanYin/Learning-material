// Compile: clang++ -std=c++23 -O2 code/dsa/algorithm/quick_sort.cpp -o code/build/quick_sort
// Run:     ./code/build/quick_sort
// Sorting: quick sort (in-place, average O(n log n)).

#include <iostream>
#include <vector>

int partition(std::vector<int>& a, int l, int r) {
    int pivot = a[r];
    int i = l;
    for (int j = l; j < r; ++j) {
        if (a[j] <= pivot) {
            std::swap(a[i], a[j]);
            ++i;
        }
    }
    std::swap(a[i], a[r]);
    return i;
}

void quick_sort(std::vector<int>& a, int l, int r) {
    if (l >= r) return;
    int p = partition(a, l, r);
    quick_sort(a, l, p - 1);
    quick_sort(a, p + 1, r);
}

int main() {
    std::vector<int> a{5, 1, 9, 3, 7, 2, 8, 4, 6};
    quick_sort(a, 0, a.size() - 1);
    for (int x : a) std::cout << x << ' ';
    std::cout << "\n";
    return 0;
}
