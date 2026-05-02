// Compile: clang++ -std=c++23 -O2 code/cpp23/string_contains.cpp -o code/build/cpp23_string_contains
// Run:     ./code/build/cpp23_string_contains
//
// Before string::contains: find(substr) != std::string::npos (or strstr on char*).
// C++23 contains: intent obvious in one call.

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

bool legacy_contains_substr(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

bool legacy_contains_char(const std::string& s, char c) {
    return s.find(c) != std::string::npos;
}

int main() {
    std::string line = "C++23 adds many quality-of-life APIs.";

    std::cout << "--- Before: find != npos ---\n";
    std::cout << "has C++23 (substr): " << std::boolalpha << legacy_contains_substr(line, "C++23") << "\n";
    std::cout << "has C++26 (substr): " << legacy_contains_substr(line, "C++26") << "\n";
    std::cout << "has 'q' (char): " << legacy_contains_char(line, 'q') << "\n";

    const char* c_line = line.c_str();
    std::cout << "strstr style: " << (std::strstr(c_line, "quality") != nullptr) << "\n";

    std::cout << "--- C++23: contains ---\n";
    std::cout << "contains \"C++23\": " << line.contains("C++23") << "\n";
    std::cout << "contains \"C++26\": " << line.contains("C++26") << "\n";
    std::cout << "contains 'q': " << line.contains('q') << "\n";

    if (line.contains("quality")) {
        std::cout << "message classification: positive\n";
    }

    std::vector<std::string> commands{
        "GET /health", "POST /login", "GET /metrics", "DELETE /user/42"};
    for (const auto& cmd : commands) {
        if (cmd.contains("GET")) {
            std::cout << "readonly command: " << cmd << "\n";
        }
    }
    return 0;
}
