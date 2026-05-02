// Compile: clang++ -std=c++20 -O2 code/cpp20/jthread_stop_token.cpp -o code/build/cpp20_jthread_stop_token
// Run:     ./code/build/cpp20_jthread_stop_token
//
// Same use case twice: background worker (ticks) + monitor (heartbeats into shared vector).
// Only the threading API differs — compare legacy vs jthread/stop_token.

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <thread>
#include <vector>

namespace {

// Shared scenario (both demos must match)
constexpr auto kWorkerSleep = std::chrono::milliseconds(100);
constexpr auto kMonitorSleep = std::chrono::milliseconds(120);
constexpr auto kMainWaitBeforeStop = std::chrono::milliseconds(350);
constexpr int kWorkerMaxTicks = 10;
constexpr int kMonitorHeartbeats = 3;

// ---------- Legacy: std::thread + std::atomic<bool> cancel + manual join ----------
void demo_legacy_two_threads() {
    std::atomic<bool> cancel{false};
    std::mutex mu;
    std::vector<int> heartbeats;

    std::thread worker([&]() {
        int tick = 0;
        while (!cancel.load()) {
            std::this_thread::sleep_for(kWorkerSleep);
            std::cout << "  [legacy] worker tick " << ++tick << "\n";
            if (tick >= kWorkerMaxTicks) {
                break;
            }
        }
        std::cout << "  [legacy] worker done\n";
    });

    std::thread monitor([&]() {
        int counter = 0;
        while (!cancel.load() && counter < kMonitorHeartbeats) {
            std::this_thread::sleep_for(kMonitorSleep);
            std::lock_guard<std::mutex> lock(mu);
            heartbeats.push_back(++counter);
        }
        std::cout << "  [legacy] monitor done\n";
    });

    std::this_thread::sleep_for(kMainWaitBeforeStop);
    cancel = true;

    worker.join();
    monitor.join();

    {
        std::lock_guard<std::mutex> lock(mu);
        std::cout << "  [legacy] monitor heartbeats: ";
        for (int v : heartbeats) {
            std::cout << v << ' ';
        }
        std::cout << "\n";
    }
}

// ---------- C++20: std::jthread + stop_token (same scenario) ----------
void demo_jthread_two_threads() {
    std::mutex mu;
    std::vector<int> heartbeats;

    std::jthread worker([](std::stop_token token) {
        int tick = 0;
        while (!token.stop_requested()) {
            std::this_thread::sleep_for(kWorkerSleep);
            std::cout << "  [jthread] worker tick " << ++tick << "\n";
            if (tick >= kWorkerMaxTicks) {
                break;
            }
        }
        std::cout << "  [jthread] worker done\n";
    });

    std::jthread monitor([&](std::stop_token token) {
        int counter = 0;
        while (!token.stop_requested() && counter < kMonitorHeartbeats) {
            std::this_thread::sleep_for(kMonitorSleep);
            std::lock_guard<std::mutex> lock(mu);
            heartbeats.push_back(++counter);
        }
        std::cout << "  [jthread] monitor done\n";
    });

    std::this_thread::sleep_for(kMainWaitBeforeStop);
    worker.request_stop();
    monitor.request_stop();
    // destructors join

    {
        std::lock_guard<std::mutex> lock(mu);
        std::cout << "  [jthread] monitor heartbeats: ";
        for (int v : heartbeats) {
            std::cout << v << ' ';
        }
        std::cout << "\n";
    }
}

}  // namespace

int main() {
    std::cout << "\n========================================================\n";
    std::cout << "__cplusplus / compiler standard macro\n";
    std::cout << "========================================================\n";
    std::cout << "__cplusplus: " << __cplusplus << "\n";

    std::cout << "\n========================================================\n";
    std::cout << "PART 1 — same use case: std::thread + atomic cancel\n";
    std::cout << "========================================================\n";
    demo_legacy_two_threads();

    std::cout << "\n========================================================\n";
    std::cout << "PART 2 — same use case: std::jthread + stop_token\n";
    std::cout << "========================================================\n";
    demo_jthread_two_threads();

    std::cout << "\n========================================================\n";
    std::cout << "done\n";
    std::cout << "========================================================\n";
    return 0;
}
