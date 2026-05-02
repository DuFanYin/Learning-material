// Compile: clang++ -std=c++20 -O2 code/cpp20/spaceship.cpp -o code/build/cpp20_spaceship
// Run:     ./code/build/cpp20_spaceship
//
// Before <=>: hand-write operator== / operator< / relational ops (easy to get inconsistent).
// C++20 <=>: one operator<=> + default gives consistent ordering.

#include <algorithm>
#include <compare>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

// ---------- Legacy: explicit relational operators ----------
struct VersionLegacy {
    int major{};
    int minor{};
    int patch{};
};

inline bool operator==(const VersionLegacy& a, const VersionLegacy& b) {
    return a.major == b.major && a.minor == b.minor && a.patch == b.patch;
}
inline bool operator<(const VersionLegacy& a, const VersionLegacy& b) {
    if (a.major != b.major) return a.major < b.major;
    if (a.minor != b.minor) return a.minor < b.minor;
    return a.patch < b.patch;
}

// ---------- C++20: defaulted <=> ----------
struct Version {
    int major{};
    int minor{};
    int patch{};
    auto operator<=>(const Version&) const = default;
};

struct User {
    int id{};
    std::string name;
    std::strong_ordering operator<=>(const User& other) const {
        return id <=> other.id;
    }
    bool operator==(const User& other) const = default;
};

int main() {
    std::cout << "--- Before: explicit == and < ---\n";
    VersionLegacy vl1{1, 2, 0}, vl2{1, 3, 0};
    std::cout << "legacy v1 < v2: " << std::boolalpha << (vl1 < vl2) << "\n";

    std::cout << "--- C++20: defaulted <=> ---\n";
    Version v1{1, 2, 0};
    Version v2{1, 3, 0};
    std::cout << "v1 < v2: " << (v1 < v2) << "\n";

    User alice{100, "Alice"};
    User bob{200, "Bob"};
    std::cout << "alice < bob (by id): " << (alice < bob) << "\n";
    std::cout << "alice == bob: " << (alice == bob) << "\n";

    std::vector<Version> versions{{2, 0, 0}, {1, 9, 9}, {2, 0, 1}, {1, 10, 0}};
    std::ranges::sort(versions);
    std::cout << "sorted versions: ";
    for (const auto& v : versions) {
        std::cout << v.major << '.' << v.minor << '.' << v.patch << ' ';
    }
    std::cout << "\n";

    std::cout << "--- Before: std::sort + tuple tie on legacy type ---\n";
    std::vector<VersionLegacy> leg{{2, 0, 0}, {1, 9, 9}};
    std::sort(leg.begin(), leg.end(), [](const VersionLegacy& a, const VersionLegacy& b) {
        return std::tie(a.major, a.minor, a.patch) < std::tie(b.major, b.minor, b.patch);
    });
    std::cout << "legacy sorted: " << leg[0].major << '.' << leg[0].minor << '.' << leg[0].patch << " ...\n";
    return 0;
}
