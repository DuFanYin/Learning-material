// Compile: clang++ -std=c++20 -O2 code/cpp20/concepts.cpp -o code/build/cpp20_concepts
// Run:     ./code/build/cpp20_concepts
//
// Realistic use: constrain generic helpers (lerp, rolling average, clamp) so wrong types
// fail at compile time with clear errors — vs enable_if or concrete overloads only.

#include <array>
#include <concepts>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <vector>

// ---------- Legacy: lerp with enable_if (common in pre-C++20 codebases) ----------
template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, T>
lerp_legacy(T a, T b, T t) {
    return a + (b - a) * t;
}

// ---------- C++20: std::floating_point (standard concept) ----------
template <std::floating_point T>
constexpr T lerp(T a, T b, T t) {
    return a + (b - a) * t;
}

// ---------- Legacy: concrete type only — typical before "proper" generic constraints ----------
double mean_first_n_legacy(const std::vector<double>& samples, std::size_t n) {
    if (n == 0 || samples.size() < n) {
        throw std::invalid_argument("bad range");
    }
    double s = 0;
    for (std::size_t i = 0; i < n; ++i) {
        s += samples[i];
    }
    return s / static_cast<double>(n);
}

// ---------- C++20: concept matches vector, array, etc. with double elements ----------
template <typename C>
concept DoubleRandomAccess = requires(const C& c, std::size_t i) {
    typename C::value_type;
    requires std::same_as<typename C::value_type, double>;
    { c.size() } -> std::convertible_to<std::size_t>;
    { c[i] } -> std::convertible_to<double>;
};

template <DoubleRandomAccess C>
double mean_first_n(const C& samples, std::size_t n) {
    if (n == 0 || samples.size() < n) {
        throw std::invalid_argument("bad range");
    }
    double s = 0;
    for (std::size_t i = 0; i < n; ++i) {
        s += samples[i];
    }
    return s / static_cast<double>(n);
}

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

// Config / game: clamp numeric settings
template <Number T>
constexpr T clamp(T v, T lo, T hi) {
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;
}

int main() {
    std::cout << "__cplusplus: " << __cplusplus << "\n";

    std::cout << "--- Interpolation (graphics / animation) ---\n";
    std::cout << "lerp_legacy(0, 100, 0.25) = " << lerp_legacy(0.0, 100.0, 0.25) << "\n";
    std::cout << "lerp (floating_point)      = " << lerp(0.0, 100.0, 0.25) << "\n";

    std::cout << "--- Rolling mean (telemetry): legacy vs generic ---\n";
    std::vector<double> series{1.0, 2.0, 3.0, 4.0, 5.0};
    std::cout << "legacy(vector<double> only): " << mean_first_n_legacy(series, 3) << "\n";
    std::cout << "concept (vector/array<double>): " << mean_first_n(series, 3) << "\n";

    std::array<double, 4> arr{10, 20, 30, 40};
    std::cout << "concept + std::array<double>: " << mean_first_n(arr, 2) << "\n";

    std::cout << "--- Clamp (bounded numeric settings) ---\n";
    std::cout << "clamp(150, 0, 100) = " << clamp(150, 0, 100) << "\n";

    static_assert(std::floating_point<double>);
    static_assert(DoubleRandomAccess<std::vector<double>>);
    static_assert(DoubleRandomAccess<std::array<double, 4>>);
    static_assert(!DoubleRandomAccess<std::vector<int>>);

    return 0;
}
