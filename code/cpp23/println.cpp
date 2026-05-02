// Compile: clang++ -std=c++23 -O2 code/cpp23/println.cpp -o code/build/cpp23_println
// Run:     ./code/build/cpp23_println
//
// Before std::print: iostream << chaining, or snprintf into buffer for formatting.
// C++23 std::print / std::println: format string + arguments (similar spirit to fmt).

#include <cstdio>
#include <iostream>
#include <print>
#include <string>
#include <tuple>
#include <vector>

int main() {
    std::string name = "Alice";
    int score = 97;
    std::vector<int> top3{97, 95, 93};

    std::cout << "--- Before: iostream ---\n";
    std::cout << "student: " << name << ", score: " << score << "\n";
    std::cout << "top3: ";
    for (std::size_t i = 0; i < top3.size(); ++i) {
        std::cout << top3[i] << (i + 1 == top3.size() ? "\n" : ", ");
    }

    char buf[128];
    std::snprintf(buf, sizeof(buf), "%s scored %d", name.c_str(), score);
    std::cout << "snprintf line: " << buf << "\n";

    std::cout << "--- C++23: std::println / std::print ---\n";
    std::println("student: {}, score: {}", name, score);
    std::print("top3: ");
    for (std::size_t i = 0; i < top3.size(); ++i) {
        std::print("{}{}", top3[i], i + 1 == top3.size() ? "\n" : ", ");
    }

    std::vector<std::tuple<int, std::string, double>> rows{
        {1, "CPU", 63.5}, {2, "RAM", 71.2}, {3, "Disk", 54.0}};
    std::println("{:>3} | {:<8} | {:>6}", "id", "metric", "value");
    for (const auto& [id, metric, value] : rows) {
        std::println("{:>3} | {:<8} | {:>6.1f}", id, metric, value);
    }
    return 0;
}
