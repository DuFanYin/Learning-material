// Compile: clang++ -std=c++23 -O2 code/cpp23/optional_monadic.cpp -o code/build/cpp23_optional_monadic
// Run:     ./code/build/cpp23_optional_monadic
//
// Before C++23 monadic optional: nested if / early returns to chain optional steps.
// C++23: and_then / transform / or_else compose without pyramid code.

#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <string>

std::optional<std::string> sanitize_name(std::string name) {
    name.erase(std::remove_if(name.begin(), name.end(), [](unsigned char c) {
                   return std::isspace(c) != 0;
               }),
               name.end());
    if (name.empty()) {
        return std::nullopt;
    }
    return name;
}

std::string to_upper(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return s;
}

// ---------- Legacy: nested if ----------
std::optional<std::string> pipeline_legacy(std::optional<std::string> raw) {
    if (!raw) return std::nullopt;
    auto cleaned = sanitize_name(*raw);
    if (!cleaned) return std::nullopt;
    return to_upper(*cleaned);
}

std::optional<std::string> pipeline_legacy_with_default(std::optional<std::string> raw) {
    auto r = pipeline_legacy(raw);
    if (r) return r;
    return std::string{"DEFAULT"};
}

int main() {
    std::cout << "--- Before: nested if chains ---\n";
    auto legacy = pipeline_legacy(std::optional<std::string>{"  cpp 23  "});
    if (legacy) {
        std::cout << "legacy normalized: " << *legacy << "\n";
    }
    std::cout << "legacy default path: " << *pipeline_legacy_with_default(std::string{"     "}) << "\n";

    std::cout << "--- C++23: monadic optional ---\n";
    std::optional<std::string> raw = std::string("  cpp 23  ");

    auto result = raw
        .and_then(sanitize_name)
        .transform([](std::string s) {
            for (char& c : s) {
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            }
            return s;
        })
        .or_else([] {
            std::cout << "name missing, default value used\n";
            return std::optional<std::string>{"DEFAULT"};
        });

    if (result) {
        std::cout << "normalized name: " << *result << "\n";
    }

    std::optional<std::string> missing = std::string("     ");
    auto repaired = missing.and_then(sanitize_name).or_else([] {
        return std::optional<std::string>{"DEFAULT"};
    });
    std::cout << "repaired name: " << repaired.value() << "\n";
    return 0;
}
