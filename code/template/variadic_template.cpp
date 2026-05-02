// Compile: clang++ -std=c++23 -O2 code/template/variadic_template.cpp -o code/build/variadic_template
// Run:     ./code/build/variadic_template
// Topic:   Variadic template + fold expression + compile-time checks.

#include <concepts>
#include <iostream>
#include <string>
#include <tuple>

template <std::integral... Args>
auto sum(Args... args) {
    return (args + ...);
}

template <typename... Args>
void print_all(const Args&... args) {
    ((std::cout << args << ' '), ...);
    std::cout << "\n";
}

template <typename... Args>
auto make_csv_row(const Args&... args) {
    std::string row;
    bool first = true;
    auto append = [&](const auto& x) {
        if (!first) row += ",";
        first = false;
        row += std::to_string(x);
    };
    (append(args), ...);
    return row;
}

template <>
auto make_csv_row<std::string, int, int>(const std::string& name, const int& score, const int& rank) {
    return name + "," + std::to_string(score) + "," + std::to_string(rank);
}

int main() {
    std::cout << "sum ints: " << sum(1, 2, 3, 4) << "\n";
    print_all("hello", 42, 3.14, std::string("template"));
    std::cout << "csv numeric: " << make_csv_row(10, 20, 30) << "\n";
    std::cout << "csv mixed:   " << make_csv_row(std::string("alice"), 98, 1) << "\n";
    auto packed = std::tuple{1, 2, 3, 4};
    std::cout << "tuple size = " << std::tuple_size_v<decltype(packed)> << "\n";
    return 0;
}
