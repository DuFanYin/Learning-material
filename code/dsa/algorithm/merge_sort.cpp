// Compile: clang++ -std=c++23 -O2 code/dsa/algorithm/merge_sort.cpp -o code/build/merge_sort
// Run:     ./code/build/merge_sort
// Sorting: merge sort (stable O(n log n)).

#include <iostream>
#include <vector>

void merge(std::vector<int>& a, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    std::vector<int> left(n1), right(n2);
    for (int i = 0; i < n1; ++i) left[i] = a[l + i];
    for (int i = 0; i < n2; ++i) right[i] = a[m + 1 + i];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) a[k++] = left[i++];
        else a[k++] = right[j++];
    }
    while (i < n1) a[k++] = left[i++];
    while (j < n2) a[k++] = right[j++];
}

void merge_sort(std::vector<int>& a, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    merge_sort(a, l, m);
    merge_sort(a, m + 1, r);
    merge(a, l, m, r);
}

int main() {
    std::vector<int> a{8, 4, 2, 9, 5, 7, 1, 3, 6};
    merge_sort(a, 0, a.size() - 1);
    for (int x : a) std::cout << x << ' ';
    std::cout << "\n";
    return 0;
}
