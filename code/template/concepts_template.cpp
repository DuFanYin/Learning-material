// Compile: clang++ -std=c++23 -O2 code/template/concepts_template.cpp -o code/build/concepts_template
// Run:     ./code/build/concepts_template
// Topic:   C++20/23 concepts-constrained generic algorithms.

#include <concepts>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <Number T>
T clamp_value(T x, T lo, T hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

template <typename C>
concept HasSizeAndIndex = requires(C c, int i) {
    { c.size() } -> std::convertible_to<int>;
    c[i];
};

template <HasSizeAndIndex C, typename Pred>
int first_index_if(const C& c, Pred pred) {
    for (int i = 0; i < c.size(); ++i) {
        if (pred(c[i])) return i;
    }
    return -1;
}

int main() {
    std::cout << clamp_value(10, 0, 5) << "\n";
    std::cout << clamp_value(2.7, 0.0, 2.5) << "\n";

    std::vector<int> nums{4, 8, 15, 16, 23, 42};
    int idx = first_index_if(nums, [](int x) { return x > 20; });
    std::cout << "first > 20 index: " << idx << "\n";

    std::string text = "templates";
    int vowel_idx = first_index_if(text, [](char c) {
        return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
    });
    std::cout << "first vowel index: " << vowel_idx << "\n";
    return 0;
}
