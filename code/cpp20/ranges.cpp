// Compile: clang++ -std=c++20 -O2 code/cpp20/ranges.cpp -o code/build/cpp20_ranges
// Run:     ./code/build/cpp20_ranges
//
// Before std::views: explicit loops + temporary vectors, or Boost.Range-style adapters.
// C++20 ranges: lazy composition with | filter | transform | take.

#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

void legacy_even_squares_first5(const std::vector<int>& values, std::vector<int>& out) {
    out.clear();
    int taken = 0;
    for (int x : values) {
        if (x % 2 != 0) continue;
        int sq = x * x;
        out.push_back(sq);
        if (++taken >= 5) break;
    }
}

int main() {
    std::vector<int> values(20);
    std::iota(values.begin(), values.end(), 1);

    std::cout << "--- Before: manual loop collecting results ---\n";
    std::vector<int> legacy_out;
    legacy_even_squares_first5(values, legacy_out);
    std::cout << "legacy first 5 even squares: ";
    for (int v : legacy_out) std::cout << v << ' ';
    std::cout << "\n";

    std::cout << "--- C++20: views pipeline ---\n";
    auto pipeline = values | std::views::filter([](int x) { return x % 2 == 0; })
                     | std::views::transform([](int x) { return x * x; })
                     | std::views::take(5);
    std::cout << "views first 5 even squares: ";
    for (int v : pipeline) {
        std::cout << v << ' ';
    }
    std::cout << "\n";

    std::vector<std::string> names{"alice", "bob", "charlie", "david"};
    auto long_names = names | std::views::filter([](const std::string& s) { return s.size() >= 5; })
                     | std::views::transform([](const std::string& s) { return s + "_ok"; });
    std::cout << "long names tagged: ";
    for (const auto& n : long_names) {
        std::cout << n << ' ';
    }
    std::cout << "\n";
    return 0;
}
