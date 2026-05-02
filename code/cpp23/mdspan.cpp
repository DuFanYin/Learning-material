// Compile: clang++ -std=c++23 -O2 code/cpp23/mdspan.cpp -o code/build/cpp23_mdspan
// Run:     ./code/build/cpp23_mdspan
//
// One scenario: daily returns in a row-major matrix [day × asset] as a single flat buffer.
//
// Old way: you store (pointer, #columns, #rows) and index with linear layout:
//   row-major => element (row r, col c) is at index (r * ncols + c).
//   Every helper must receive ncols (stride between rows) or you risk wrong offsets.
//
// New way: std::mdspan wraps the same pointer with 2D extents; you index m[r, c].
//   Shape lives on the view; loops read naturally and you still pass one span into helpers.

#include <cmath>
#include <iostream>
#include <limits>
#include <mdspan>
#include <vector>

namespace {

constexpr std::size_t kDays = 120;
constexpr std::size_t kAssets = 24;

void fill_synthetic_returns(std::vector<double>& buf) {
    buf.resize(kDays * kAssets);
    for (std::size_t d = 0; d < kDays; ++d) {
        for (std::size_t a = 0; a < kAssets; ++a) {
            buf[d * kAssets + a] =
                1e-4 * std::sin(static_cast<double>(d) * 0.07 + static_cast<double>(a) * 0.11);
        }
    }
}

// Legacy: cannot infer shape from ptr — caller must pass ncols (row stride) and nrows.
double mean_roi_legacy(const double* data, std::size_t ncols, std::size_t nrows, std::size_t row0,
                       std::size_t col0, std::size_t roi_rows, std::size_t roi_cols) {
    if (row0 + roi_rows > nrows || col0 + roi_cols > ncols) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    long double sum = 0;
    for (std::size_t r = row0; r < row0 + roi_rows; ++r) {
        for (std::size_t c = col0; c < col0 + roi_cols; ++c) {
            sum += static_cast<long double>(data[r * ncols + c]);  // manual row-major
        }
    }
    return static_cast<double>(sum / static_cast<long double>(roi_rows * roi_cols));
}

// Same logic: mdspan carries extents; use multidimensional subscript (C++23).
double mean_roi_mdspan(std::mdspan<const double, std::dextents<std::size_t, 2>> R, std::size_t row0,
                       std::size_t col0, std::size_t roi_rows, std::size_t roi_cols) {
    if (row0 + roi_rows > R.extent(0) || col0 + roi_cols > R.extent(1)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    long double sum = 0;
    for (std::size_t r = row0; r < row0 + roi_rows; ++r) {
        for (std::size_t c = col0; c < col0 + roi_cols; ++c) {
            sum += static_cast<long double>(R[r, c]);
        }
    }
    return static_cast<double>(sum / static_cast<long double>(roi_rows * roi_cols));
}

}  // namespace

int main() {
    std::cout << "\n========================================================\n";
    std::cout << "__cplusplus: " << __cplusplus << "\n";
    std::cout << "========================================================\n";

    std::cout << "\n========================================================\n";
    std::cout << "Returns matrix: legacy linear index vs mdspan\n";
    std::cout << "========================================================\n";

    std::vector<double> buf;
    fill_synthetic_returns(buf);

    const std::size_t d0 = 40;
    const std::size_t a0 = 6;
    const std::size_t win_days = 20;
    const std::size_t n_assets = 8;

    std::cout << "Matrix: " << kDays << " days × " << kAssets << " assets (row-major in one vector).\n";
    std::cout << "Task: mean return over days [" << d0 << ", " << (d0 + win_days) << "), assets [" << a0
              << ", " << (a0 + n_assets) << ").\n\n";

    std::cout << "Old method:\n";
    std::cout << "  - Index element (day d, asset a) as buf[d * kAssets + a].\n";
    std::cout << "  - mean_roi_legacy(buf.data(), kAssets, kDays, ...) passes stride (ncols) + sizes.\n";
    const double old_way =
        mean_roi_legacy(buf.data(), kAssets, kDays, d0, a0, win_days, n_assets);
    std::cout << "  => mean = " << old_way << "\n\n";

    std::cout << "mdspan method:\n";
    std::cout << "  - Same buffer: mdspan R(buf.data(), kDays, kAssets)  // extents: rows, cols.\n";
    std::cout << "  - Index as R[day, asset] (no manual multiply).\n";
    std::cout << "  - mean_roi_mdspan(R, ...) uses R.extent(0/1) for bounds.\n";
    std::mdspan<const double, std::dextents<std::size_t, 2>> R(buf.data(), kDays, kAssets);
    const double new_way = mean_roi_mdspan(R, d0, a0, win_days, n_assets);
    std::cout << "  => mean = " << new_way << "\n\n";

    const std::size_t dd = 55;
    const std::size_t aa = 9;
    std::cout << "Same cell: legacy buf[" << dd << "*" << kAssets << "+" << aa << "] = "
              << buf[dd * kAssets + aa] << ", mdspan R[" << dd << "," << aa << "] = " << R[dd, aa] << "\n";

    std::cout << "\n========================================================\n";
    std::cout << "done\n";
    std::cout << "========================================================\n";
    return 0;
}
