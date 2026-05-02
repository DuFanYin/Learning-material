// Compile: clang++ -std=c++20 -O2 code/cpp20/span.cpp -o code/build/cpp20_span
// Run:     ./code/build/cpp20_span
//
// Before std::span: (pointer, size) pairs or overloads for vector vs array vs raw buffer.
// C++20 span: one non-owning view type for any contiguous sequence.

#include <array>
#include <iostream>
#include <span>
#include <vector>

// ---------- Legacy: separate entry points or pointer + length ----------
void normalize_ptr_len(int* p, std::size_t n) {
    if (!n) return;
    int first = p[0];
    for (std::size_t i = 0; i < n; ++i) p[i] -= first;
}

void normalize_vector(std::vector<int>& v) {
    if (v.empty()) return;
    int first = v.front();
    for (int& x : v) x -= first;
}

void print_legacy(const std::vector<int>& v, const char* label) {
    std::cout << label << ": ";
    for (int x : v) std::cout << x << ' ';
    std::cout << "\n";
}

// ---------- C++20: std::span ----------
void normalize(std::span<int> s) {
    if (s.empty()) return;
    int first = s.front();
    for (int& x : s) x -= first;
}

void print_span(std::span<const int> s, const char* label) {
    std::cout << label << ": ";
    for (int v : s) std::cout << v << ' ';
    std::cout << "\n";
}

int sum_window(std::span<const int> s) {
    int total = 0;
    for (int v : s) total += v;
    return total;
}

int main() {
    std::cout << "--- Before: ptr+len vs vector overload ---\n";
    std::vector<int> v1{10, 12, 17, 21};
    std::array<int, 4> a1{30, 31, 35, 40};
    normalize_vector(v1);
    normalize_ptr_len(a1.data(), a1.size());
    print_legacy(v1, "legacy vec");
    print_legacy(std::vector<int>(a1.begin(), a1.end()), "legacy arr as vec");

    std::cout << "--- C++20: single normalize(span) ---\n";
    std::vector<int> vec{10, 12, 17, 21};
    std::array<int, 4> arr{30, 31, 35, 40};
    normalize(vec);
    normalize(arr);
    print_span(vec, "normalized vector");
    print_span(arr, "normalized array");

    std::span<const int> mid(vec.data() + 1, 2);
    std::cout << "sum of middle window: " << sum_window(mid) << "\n";
    return 0;
}
