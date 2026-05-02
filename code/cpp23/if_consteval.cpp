// Compile: clang++ -std=c++23 -O2 code/cpp23/if_consteval.cpp -o code/build/cpp23_if_consteval
// Run:     ./code/build/cpp23_if_consteval
//
// Before if consteval: split into constexpr-only helpers vs runtime overloads,
// or MACRO tricks — awkward when one name must mean two behaviors.
// C++23 if consteval: one function, explicit compile-time vs runtime branch.

#include <iostream>

// ---------- Legacy pattern: two overloads ----------
constexpr int compute_compiletime(int x) {
    return x * x;
}

int compute_runtime(int x) {
    return x * x + 1;
}

// Caller must pick the right overload (error-prone).
constexpr int use_legacy_constexpr(int x) {
    return compute_compiletime(x);
}

// ---------- C++23: if consteval inside one constexpr function ----------
constexpr int compute(int x) {
    if consteval {
        return x * x;
    } else {
        return x * x + 1;
    }
}

consteval int compile_time_hash_seed() {
    return 131;
}

template <int N>
constexpr int compile_time_power2() {
    if consteval {
        return 1 << N;
    } else {
        return -1;
    }
}

int main() {
    std::cout << "--- Before: separate constexpr vs runtime functions ---\n";
    constexpr int lc = use_legacy_constexpr(5);
    int lr = compute_runtime(5);
    std::cout << "constexpr path (manual overload): " << lc << "\n";
    std::cout << "runtime path (separate function): " << lr << "\n";

    std::cout << "--- C++23: single compute() with if consteval ---\n";
    constexpr int c = compute(5);
    int r = compute(5);
    std::cout << "constexpr compute(5) = " << c << "\n";
    std::cout << "runtime   compute(5) = " << r << "\n";

    constexpr int seed = compile_time_hash_seed();
    constexpr int block_size = compile_time_power2<8>();
    std::cout << "compile-time seed = " << seed << "\n";
    std::cout << "compile-time block_size = " << block_size << "\n";
    return 0;
}
