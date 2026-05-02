// Compile: clang++ -std=c++23 -O2 code/cpp23/expected.cpp -o code/build/cpp23_expected
// Run:     ./code/build/cpp23_expected
//
// What this demo shows:
// 1) Before std::expected: typical patterns (bool+out, optional, exceptions).
// 2) std::expected: one return type carries success value OR error reason.

#include <expected>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

// ---------- Legacy style A: bool + output parameter + separate error string ----------
// Common in C APIs / older C++; caller must pass three parameters.
bool parse_positive_int_legacy(std::string_view text, int& out_value, std::string& err) {
    err.clear();
    if (text.empty()) {
        err = "input is empty";
        return false;
    }
    int value = 0;
    for (char c : text) {
        if (c < '0' || c > '9') {
            err = "invalid character";
            return false;
        }
        value = value * 10 + (c - '0');
    }
    if (value <= 0) {
        err = "must be positive";
        return false;
    }
    out_value = value;
    return true;
}

// ---------- Legacy style B: std::optional (success/fail only, no error message) ----------
std::optional<int> parse_positive_int_optional_only(std::string_view text) {
    if (text.empty()) {
        return std::nullopt;
    }
    int value = 0;
    for (char c : text) {
        if (c < '0' || c > '9') {
            return std::nullopt;
        }
        value = value * 10 + (c - '0');
    }
    if (value <= 0) {
        return std::nullopt;
    }
    return value;
}

// ---------- Legacy style C: exceptions (works but not ideal for "expected" failure paths) ----------
int parse_positive_int_throw(std::string_view text) {
    if (text.empty()) {
        throw std::invalid_argument("input is empty");
    }
    int value = 0;
    for (char c : text) {
        if (c < '0' || c > '9') {
            throw std::invalid_argument("invalid character");
        }
        value = value * 10 + (c - '0');
    }
    if (value <= 0) {
        throw std::invalid_argument("must be positive");
    }
    return value;
}

// ---------- C++23: std::expected (value OR typed error in one object) ----------
std::expected<int, std::string> parse_positive_int(std::string_view text) {
    if (text.empty()) {
        return std::unexpected("input is empty");
    }
    int value = 0;
    for (char c : text) {
        if (c < '0' || c > '9') {
            return std::unexpected("invalid character");
        }
        value = value * 10 + (c - '0');
    }
    if (value <= 0) {
        return std::unexpected("must be positive");
    }
    return value;
}

std::expected<int, std::string> compute_discounted_price(
    int base_price, std::function<std::expected<int, std::string>(int)> discount_fn) {
    auto discount = discount_fn(base_price);
    if (!discount) {
        return std::unexpected(discount.error());
    }
    return base_price - *discount;
}

int main() {
    const std::string sample = "12x";

    std::cout << "--- Before: bool + int& + std::string& err ---\n";
    int v = 0;
    std::string err;
    if (parse_positive_int_legacy(sample, v, err)) {
        std::cout << "value: " << v << "\n";
    } else {
        std::cout << "error: " << err << "\n";
    }

    std::cout << "--- Before: std::optional (same failure, no reason) ---\n";
    if (auto o = parse_positive_int_optional_only(sample)) {
        std::cout << "value: " << *o << "\n";
    } else {
        std::cout << "failed (optional cannot tell why)\n";
    }

    std::cout << "--- Before: exception ---\n";
    try {
        int x = parse_positive_int_throw(sample);
        std::cout << "value: " << x << "\n";
    } catch (const std::exception& e) {
        std::cout << "exception: " << e.what() << "\n";
    }

    std::cout << "--- C++23: std::expected ---\n";
    if (auto r = parse_positive_int(sample)) {
        std::cout << "value: " << *r << "\n";
    } else {
        std::cout << "error: " << r.error() << "\n";
    }

    std::cout << "\n--- Same inputs, expected API (parser demo) ---\n";
    for (std::string s : {"42", "0", "12x", ""}) {
        auto r = parse_positive_int(s);
        if (r) {
            std::cout << "input [" << s << "] => value: " << *r << "\n";
        } else {
            std::cout << "input [" << s << "] => error: " << r.error() << "\n";
        }
    }

    std::cout << "\n--- Business flow with expected ---\n";
    auto discount_fn = [](int base) -> std::expected<int, std::string> {
        if (base < 100) {
            return std::unexpected("price too low for campaign");
        }
        return 20;
    };
    auto final_price = compute_discounted_price(150, discount_fn);
    if (final_price) {
        std::cout << "discounted price: " << *final_price << "\n";
    } else {
        std::cout << "discount calc failed: " << final_price.error() << "\n";
    }
    return 0;
}
