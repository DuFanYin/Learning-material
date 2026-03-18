// Build:
//   mkdir -p build
//   clang++ -std=c++20 -O2 codes/src/concurrency/memory_order_example.cpp -o build/memory_order_example
// Run:
//   ./build/memory_order_example
//
// 这个例子用几套彼此独立的代码块，对比不同 memory_order：
// 1) 完全没有同步（data race）
// 2) atomic<bool> + relaxed：只有原子性，没有同步
// 3) release+acquire：正确的发布/接收
// 4) seq_cst：最强顺序，推理最直观
// 5) relaxed counter：合法的 relaxed 用法（纯计数）

#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace case1_naive {

    int data = 0;
    bool ready = false;

    void producer() {
        data = 42;
        ready = true;  // 普通 bool，无同步
    }

    void consumer() {
        while (!ready) {
            // busy-wait
        }
        // 标准层面：这是 data race，行为未定义
        std::cout << "  [naive]          data = " << data
                << "   (data race, undefined by C++ standard)\n";
    }

}  // namespace case1_naive

namespace case2_relaxed_flag {

    std::atomic<bool> ready{false};
    int data = 0;

    void producer() {
        data = 42;
        ready.store(true, std::memory_order_relaxed);
    }

    void consumer() {
        while (!ready.load(std::memory_order_relaxed)) {
            // busy-wait
        }
        // 在弱内存模型上，这里“理论上可能”看到 data != 42
        std::cout << "  [relaxed]        data = " << data
                << "   (no happens-before; value not guaranteed)\n";
    }

}  // namespace case2_relaxed_flag

namespace case3_release_acquire {

    std::atomic<bool> ready{false};
    int data = 0;

    void producer() {
        data = 42;
        ready.store(true, std::memory_order_release);
    }

    void consumer() {
        while (!ready.load(std::memory_order_acquire)) {
            // busy-wait
        }
        assert(data == 42);
        std::cout << "  [release/acquire] data = " << data
                << "   (guaranteed 42 via release/acquire)\n";
    }

}  // namespace case3_release_acquire

namespace case4_seq_cst {

    std::atomic<bool> ready{false};
    int data = 0;

    void producer() {
        data = 42;
        ready.store(true, std::memory_order_seq_cst);
    }

    void consumer() {
        while (!ready.load(std::memory_order_seq_cst)) {
            // busy-wait
        }
        assert(data == 42);
        std::cout << "  [seq_cst]        data = " << data
                << "   (guaranteed 42; participates in global seq_cst order)\n";
    }

}  // namespace case4_seq_cst

namespace case5_relaxed_counter {

    std::atomic<long> counter{0};

    void worker(int iters) {
        for (int i = 0; i < iters; ++i) {
            counter.fetch_add(1, std::memory_order_relaxed);
        }
    }

}  // namespace case5_relaxed_counter

int main() {
    std::cout << "==============================\n";
    std::cout << "1) Naive (no atomics, data race)\n";
    std::cout << "==============================\n";
    {
        std::thread t1(case1_naive::producer);
        std::thread t2(case1_naive::consumer);
        t1.join();
        t2.join();
    }

    std::cout << "\n==============================\n";
    std::cout << "2) atomic<bool> with relaxed (only atomicity, no ordering)\n";
    std::cout << "==============================\n";
    {
        std::thread t1(case2_relaxed_flag::producer);
        std::thread t2(case2_relaxed_flag::consumer);
        t1.join();
        t2.join();
    }

    std::cout << "\n==============================\n";
    std::cout << "3) release + acquire (proper publish/subscribe)\n";
    std::cout << "==============================\n";
    {
        std::thread t1(case3_release_acquire::producer);
        std::thread t2(case3_release_acquire::consumer);
        t1.join();
        t2.join();
    }

    std::cout << "\n==============================\n";
    std::cout << "4) seq_cst (strongest order; easiest to reason about)\n";
    std::cout << "==============================\n";
    {
        std::thread t1(case4_seq_cst::producer);
        std::thread t2(case4_seq_cst::consumer);
        t1.join();
        t2.join();
    }

    std::cout << "\n==============================\n";
    std::cout << "5) relaxed counter (合法用法：纯计数)\n";
    std::cout << "==============================\n";
    {
        using namespace case5_relaxed_counter;
        counter.store(0, std::memory_order_relaxed);
        const int threads = 4;
        const int iters_per_thread = 1'000'000;
        std::vector<std::thread> ts;
        ts.reserve(threads);
        for (int i = 0; i < threads; ++i) {
            ts.emplace_back(worker, iters_per_thread);
        }
        for (auto& t : ts) {
            t.join();
        }
        long expected = static_cast<long>(threads) * iters_per_thread;
        long actual = counter.load(std::memory_order_relaxed);
        std::cout << "  [relaxed counter] expected = " << expected
                  << ", actual = " << actual
                  << "   (correct use of memory_order_relaxed)\n";
    }

    return 0;
}

