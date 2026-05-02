// Compile: clang++ -std=c++23 -O2 code/dsa/algorithm/heap_sort.cpp -o code/build/heap_sort
// Run:     ./code/build/heap_sort
// Sorting: heap sort (in-place O(n log n)).

#include <iostream>
#include <vector>

void heapify(std::vector<int>& a, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && a[l] > a[largest]) largest = l;
    if (r < n && a[r] > a[largest]) largest = r;

    if (largest != i) {
        std::swap(a[i], a[largest]);
        heapify(a, n, largest);
    }
}

void heap_sort(std::vector<int>& a) {
    int n = a.size();
    for (int i = n / 2 - 1; i >= 0; --i) heapify(a, n, i);
    for (int i = n - 1; i > 0; --i) {
        std::swap(a[0], a[i]);
        heapify(a, i, 0);
    }
}

int main() {
    std::vector<int> a{10, 3, 15, 7, 8, 23, 74, 18};
    heap_sort(a);
    for (int x : a) std::cout << x << ' ';
    std::cout << "\n";
    return 0;
}
