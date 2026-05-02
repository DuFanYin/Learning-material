#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>

// Single-producer single-consumer ring buffer.
// - Lock-free for the common case.
// - Template parameter N must be power of two for mask optimization.
template <typename T, std::size_t N>
class SpscRingBuffer {
    static_assert((N & (N - 1)) == 0, "N must be power of two");
    static constexpr std::size_t kCacheLine = 64;
    static constexpr std::size_t kMask = N - 1;

    // This implementation assumes `T` is cheap to move/assign.
    // For a fully general ring buffer, consider raw storage + placement new.
    static_assert(std::is_trivially_copyable_v<T> || std::is_nothrow_move_assignable_v<T>,
                  "T should be trivially copyable or nothrow move-assignable");

public:
    // Head == consumer index, tail == producer index.
    SpscRingBuffer() : head_(0), tail_(0), head_cache_(0), tail_cache_(0) {}

    // Enqueue; returns false if buffer is full.
    bool push(const T& value) {
        return emplace(value);
    }

    bool push(T&& value) {
        return emplace(std::move(value));
    }

    // Dequeue; returns empty optional if buffer is empty.
    std::optional<T> pop() {
        std::size_t head = head_.v.load(std::memory_order_relaxed);
        if (head == tail_cache_) {
            tail_cache_ = tail_.v.load(std::memory_order_acquire);
            if (head == tail_cache_) {
                return std::nullopt;  // empty
            }
        }
        T value = std::move(buffer_[head & kMask]);
        head_.v.store(head + 1, std::memory_order_release);
        return value;
    }

    // Dequeue into `out`; returns false if buffer is empty.
    bool pop(T& out) {
        std::size_t head = head_.v.load(std::memory_order_relaxed);
        if (head == tail_cache_) {
            tail_cache_ = tail_.v.load(std::memory_order_acquire);
            if (head == tail_cache_) {
                return false;  // empty
            }
        }
        out = std::move(buffer_[head & kMask]);
        head_.v.store(head + 1, std::memory_order_release);
        return true;
    }

    bool empty() const noexcept {
        return head_.v.load(std::memory_order_acquire) ==
               tail_.v.load(std::memory_order_acquire);
    }

    bool full() const noexcept {
        return size() == N;
    }

    std::size_t size() const noexcept {
        std::size_t head = head_.v.load(std::memory_order_acquire);
        std::size_t tail = tail_.v.load(std::memory_order_acquire);
        return tail - head;
    }

private:
    struct alignas(kCacheLine) PaddedAtomicSizeT {
        std::atomic<std::size_t> v;
    };

    template <typename U>
    bool emplace(U&& value) {
        // Producer thread only.
        std::size_t tail = tail_.v.load(std::memory_order_relaxed);
        std::size_t nextTail = tail + 1;

        // Fast path: use cached consumer head to avoid an acquire load every push.
        if (nextTail - head_cache_ > N) {
            // Refresh consumer progress. Acquire pairs with consumer's release store
            // on head_ so the producer doesn't reuse a slot too early on weak memory models.
            head_cache_ = head_.v.load(std::memory_order_acquire);
            if (nextTail - head_cache_ > N) {
                return false;  // full
            }
        }

        buffer_[tail & kMask] = std::forward<U>(value);
        tail_.v.store(nextTail, std::memory_order_release);
        return true;
    }

    T buffer_[N];
    // Keep head and tail on different cache lines to reduce false sharing.
    PaddedAtomicSizeT head_;
    PaddedAtomicSizeT tail_;

    // Per-side cached indices (written/read by only one thread each).
    // Producer reads `head_cache_` to check space; consumer updates `tail_cache_` to check data.
    // This is SPSC-only: head_cache_ must only be accessed by the producer thread, and
    // tail_cache_ must only be accessed by the consumer thread.
    alignas(kCacheLine) std::size_t head_cache_;
    alignas(kCacheLine) std::size_t tail_cache_;
};

