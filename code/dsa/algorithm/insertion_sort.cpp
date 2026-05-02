// Compile: clang++ -std=c++23 -O2 code/dsa/algorithm/insertion_sort.cpp -o code/build/insertion_sort
// Run:     ./code/build/insertion_sort
// Sorting: insertion sort (good for small/nearly sorted arrays).

#include <iostream>
#include <vector>

void insertion_sort(std::vector<int>& a) {
    for (int i = 1; i < a.size(); ++i) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}

int main() {
    std::vector<int> a{7, 4, 5, 2, 9, 1};
    insertion_sort(a);
    for (int x : a) std::cout << x << ' ';
    std::cout << "\n";
    return 0;
}
