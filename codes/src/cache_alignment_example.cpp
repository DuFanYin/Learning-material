// Benchmark: cache-aligned vs non-aligned counters under false sharing.
// Build:
//   mkdir -p build
//   clang++ -std=c++20 -O2 -pthread codes/src/cache_alignment_example.cpp -o build/cache_align_bench
// Run:
//   ./build/cache_align_bench

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

// Adjust if your cache line size is different; 64B is common on x86/ARM.
constexpr std::size_t CACHE_LINE      = 64;
constexpr std::size_t ITER_PER_THREAD = 50000000;

struct Counter {
    std::atomic<std::uint64_t> value{};
};

struct alignas(CACHE_LINE) PaddedCounter {
    std::atomic<std::uint64_t> value{};
};

template <typename T>
std::uint64_t run_benchmark(const char* name, std::size_t threads) {
    std::vector<T> counters(threads);
    for (auto& c : counters) {
        c.value.store(0, std::memory_order_relaxed);
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> workers;
    workers.reserve(threads);
    for (std::size_t t = 0; t < threads; ++t) {
        workers.emplace_back([&, idx = t] {
            for (std::size_t i = 0; i < ITER_PER_THREAD; ++i) {
                counters[idx].value.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    for (auto& th : workers) {
        th.join();
    }

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::uint64_t sum = 0;
    for (auto& c : counters) {
        sum += c.value.load(std::memory_order_relaxed);
    }

    std::cout << std::left << std::setw(18) << name << ": "
              << duration << " ms"
              << " (sum=" << sum << ")\n";

    return static_cast<std::uint64_t>(duration);
}

int main() {
    unsigned hw = std::thread::hardware_concurrency();
    if (hw == 0) {
        hw = 4;
    }
    const std::size_t threads = std::min<std::size_t>(hw, 8);

    std::cout << "Cache line size assumption: " << CACHE_LINE << " bytes\n";
    std::cout << "Threads:                     " << threads << "\n";
    std::cout << "Iterations per thread:      " << ITER_PER_THREAD << "\n\n";

    auto t1 = run_benchmark<Counter>("Non-aligned", threads);
    auto t2 = run_benchmark<PaddedCounter>("Cache-aligned", threads);

    std::cout << "\nSpeed ratio (non / aligned): "
              << std::fixed << std::setprecision(2)
              << (t2 ? static_cast<double>(t1) / static_cast<double>(t2) : 0.0)
              << "x\n";

    return 0;
}

