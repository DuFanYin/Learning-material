// Build:
//   mkdir -p build
//   clang++ -std=c++20 -O2 -pthread codes/src/infra/spsc_example.cpp -o build/spsc_bench
// Run:
//   ./build/spsc_bench

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "../../includes/spsc_ring_buffer.h"

#if defined(__APPLE__)
#include <mach/mach.h>
#include <mach/thread_policy.h>
#endif

namespace {

using Clock = std::chrono::high_resolution_clock;

inline std::uint64_t ms_since(Clock::time_point start) {
    auto end = Clock::now();
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

inline void cpu_relax() {
    // Fixed to ARM: use the architectural yield hint while spinning.
    asm volatile("yield" ::: "memory");
}

// Best-effort thread "pinning":
// - macOS: uses Mach thread affinity tags (does not guarantee a specific core,
//   but encourages threads with different tags not to share the same core).
// - Linux: could use pthread_setaffinity_np for real CPU pinning (not enabled here).
inline void set_thread_affinity_tag(int tag) {
#if defined(__APPLE__)
    thread_affinity_policy_data_t policy = {tag};
    thread_port_t self = mach_thread_self();
    (void)thread_policy_set(self,
                            THREAD_AFFINITY_POLICY,
                            reinterpret_cast<thread_policy_t>(&policy),
                            THREAD_AFFINITY_POLICY_COUNT);
    mach_port_deallocate(mach_task_self(), self);
#else
    (void)tag;
#endif
}

struct Backoff {
    std::uint32_t spins = 0;

    void operator()() {
        // Favor user-space spinning: mostly pause, occasionally yield.
        // (Sleeping can dominate the cost and makes results overly scheduler-dependent.)
        for (int i = 0; i < 100; ++i) cpu_relax();
        if ((spins & 0xFFFF) == 0xFFFF) {
            std::this_thread::yield();
        }
        ++spins;
    }

    void reset() { spins = 0; }
};

inline std::uint64_t payload(std::uint64_t x) {
    // Ring buffers typically shine when payload is light and sync overhead dominates.
    // Keep it trivial: a counter value.
    return x;
}

struct Result {
    std::uint64_t ms = 0;
    std::uint64_t sum = 0;
};

Result bench_mutex_cv(std::size_t n, std::size_t batch, bool warmup) {
    std::queue<std::uint64_t> q;
    std::mutex m;

    std::uint64_t sum = 0;
    Backoff backoff_prod;
    Backoff backoff_cons;

    auto start = Clock::now();

    std::thread prod([&] {
        set_thread_affinity_tag(1);
        for (std::size_t i = 0; i < n; ++i) {
            std::lock_guard<std::mutex> lk(m);
            q.push(payload(static_cast<std::uint64_t>(i)));
        }
    });

    std::thread cons([&] {
        set_thread_affinity_tag(2);
        std::size_t received = 0;
        while (received < n) {
            bool got = false;
            std::uint64_t v = 0;
            {
                std::lock_guard<std::mutex> lk(m);
                if (!q.empty()) {
                    v = q.front();
                    q.pop();
                    got = true;
                }
            }
            if (got) {
                sum += v;
                ++received;
                backoff_cons.reset();
            } else {
                backoff_cons();
            }
        }
    });

    prod.join();
    cons.join();

    auto ms = ms_since(start);
    if (warmup) return Result{0, sum};
    return Result{ms, sum};
}

Result bench_spsc(std::size_t n, std::size_t batch, bool warmup) {
    // Capacity chosen to reduce full/empty oscillation and highlight steady-state throughput.
    // Note: large ring buffers can overflow small default thread stacks.
    // Keep capacity moderate so the object fits on the stack on typical systems.
    SpscRingBuffer<std::uint64_t, 1 << 18> rb;

    std::uint64_t sum = 0;
    Backoff backoff_prod;
    Backoff backoff_cons;

    auto start = Clock::now();

    std::thread prod([&] {
        set_thread_affinity_tag(1);
        for (std::size_t i = 0; i < n; i += batch) {
            std::size_t end = std::min(n, i + batch);
            for (std::size_t j = i; j < end; ++j) {
                std::uint64_t v = payload(static_cast<std::uint64_t>(j));
                while (!rb.push(v)) {
                    backoff_prod();
                }
                backoff_prod.reset();
            }
        }
    });

    std::thread cons([&] {
        set_thread_affinity_tag(2);
        std::size_t received = 0;
        std::uint64_t v = 0;
        while (received < n) {
            if (rb.pop(v)) {
                // Batch pop for symmetry with the mutex+cv drain behavior.
                sum += v;
                ++received;
                backoff_cons.reset();

                for (std::size_t k = 1; k < batch && received < n; ++k) {
                    if (!rb.pop(v)) {
                        break;
                    }
                    sum += v;
                    ++received;
                }
            } else {
                backoff_cons();
            }
        }
    });

    prod.join();
    cons.join();

    auto ms = ms_since(start);
    if (warmup) {
        return Result{0, sum};
    }
    return Result{ms, sum};
}

void print_result(const char* name, const Result& r, std::size_t n) {
    double secs = static_cast<double>(r.ms) / 1000.0;
    double mps = secs > 0 ? (static_cast<double>(n) / 1e6) / secs : 0.0;
    std::cout << std::left << std::setw(18) << name
              << ": " << std::setw(6) << r.ms << " ms"
              << "  " << std::fixed << std::setprecision(2) << mps << " Mmsg/s"
              << "  (sum=" << r.sum << ")\n";
}

}  // namespace

int main() {
    constexpr std::size_t N = 20'000'000;
    constexpr std::size_t BATCH = 16;

    // Warm up (cache, CPU frequency, allocator state).
    (void)bench_mutex_cv(1'000'000, BATCH, /*warmup=*/true);
    (void)bench_spsc(1'000'000, BATCH, /*warmup=*/true);

    std::cout << "N=" << N << " batch=" << BATCH << "\n";

    auto r1 = bench_mutex_cv(N, BATCH, /*warmup=*/false);
    auto r2 = bench_spsc(N, BATCH, /*warmup=*/false);

    print_result("mutex+cv queue", r1, N);
    print_result("SPSC ring buffer", r2, N);

    return 0;
}

