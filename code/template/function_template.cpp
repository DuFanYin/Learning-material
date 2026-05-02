// Compile: clang++ -std=c++23 -O2 code/template/function_template.cpp -o code/build/function_template
// Run:     ./code/build/function_template
// Topic:   Function template with custom projection + specialization.

#include <functional>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
T my_max(T a, T b) {
    return a > b ? a : b;
}

template <typename T, typename Proj = std::identity>
const T& max_by(const T& a, const T& b, Proj proj = {}) {
    return proj(a) < proj(b) ? b : a;
}

// Full specialization for const char* to avoid pointer comparison.
template <>
const char* my_max<const char*>(const char* a, const char* b) {
    return std::string(a) > std::string(b) ? a : b;
}

int main() {
    std::cout << "max int: " << my_max(3, 9) << "\n";
    std::cout << "max double: " << my_max(2.5, 1.8) << "\n";
    std::cout << "max cstr: " << my_max("apple", "banana") << "\n";

    struct Player {
        std::string name;
        int score;
        int wins;
    };
    Player a{"Alice", 91, 6}, b{"Bob", 88, 9};

    const Player& by_score = max_by(a, b, [](const Player& p) { return p.score; });
    const Player& by_wins = max_by(a, b, [](const Player& p) { return p.wins; });

    std::cout << "max by score: " << by_score.name << "\n";
    std::cout << "max by wins: " << by_wins.name << "\n";

    std::vector<std::string> words{"template", "meta", "programming"};
    const std::string& longest = max_by(words[0], words[2], [](const std::string& s) {
        return s.size();
    });
    std::cout << "longer word: " << longest << "\n";
    return 0;
}
