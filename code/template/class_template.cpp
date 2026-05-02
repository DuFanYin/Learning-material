// Compile: clang++ -std=c++23 -O2 code/template/class_template.cpp -o code/build/class_template
// Run:     ./code/build/class_template
// Topic:   Class template: small vector with custom allocator strategy.

#include <cstddef>
#include <algorithm>
#include <iostream>
#include <string>

template <typename T>
struct DefaultAllocator {
    T* allocate(std::size_t n) { return new T[n]; }
    void deallocate(T* p) { delete[] p; }
};

template <typename T, typename Alloc = DefaultAllocator<T>>
class SmallVector {
public:
    explicit SmallVector(std::size_t cap = 4) : data_(alloc_.allocate(cap)), sz_(0), cap_(cap) {}
    ~SmallVector() { alloc_.deallocate(data_); }

    // Rule of 5: deep-copy semantics.
    SmallVector(const SmallVector& other) : data_(alloc_.allocate(other.cap_)), sz_(other.sz_), cap_(other.cap_) {
        for (std::size_t i = 0; i < sz_; ++i) data_[i] = other.data_[i];
    }

    SmallVector& operator=(const SmallVector& other) {
        if (this == &other) return *this;
        T* nd = alloc_.allocate(other.cap_);
        for (std::size_t i = 0; i < other.sz_; ++i) nd[i] = other.data_[i];
        alloc_.deallocate(data_);
        data_ = nd;
        sz_ = other.sz_;
        cap_ = other.cap_;
        return *this;
    }

    SmallVector(SmallVector&& other) noexcept
        : data_(other.data_), sz_(other.sz_), cap_(other.cap_) {
        other.data_ = nullptr;
        other.sz_ = 0;
        other.cap_ = 0;
    }

    SmallVector& operator=(SmallVector&& other) noexcept {
        if (this == &other) return *this;
        alloc_.deallocate(data_);
        data_ = other.data_;
        sz_ = other.sz_;
        cap_ = other.cap_;
        other.data_ = nullptr;
        other.sz_ = 0;
        other.cap_ = 0;
        return *this;
    }

    void push_back(const T& x) {
        if (sz_ == cap_) grow();
        data_[sz_++] = x;
    }

    T& operator[](std::size_t i) { return data_[i]; }
    const T& operator[](std::size_t i) const { return data_[i]; }

    std::size_t size() const { return sz_; }
    std::size_t capacity() const { return cap_; }

private:
    void grow() {
        std::size_t next = cap_ * 2;
        T* nd = alloc_.allocate(next);
        for (std::size_t i = 0; i < sz_; ++i) nd[i] = data_[i];
        alloc_.deallocate(data_);
        data_ = nd;
        cap_ = next;
    }

    Alloc alloc_;
    T* data_;
    std::size_t sz_;
    std::size_t cap_;
};

int main() {
    SmallVector<int> nums;
    for (int x : {3, 1, 4, 1, 5, 9}) nums.push_back(x);
    std::cout << "nums size/cap: " << nums.size() << "/" << nums.capacity() << "\n";

    SmallVector<int> copied = nums;  // copy ctor
    std::cout << "copied size/cap: " << copied.size() << "/" << copied.capacity() << "\n";
    SmallVector<int> assigned;
    assigned = nums;                 // copy assign
    std::cout << "assigned size/cap: " << assigned.size() << "/" << assigned.capacity() << "\n";

    SmallVector<std::string> names(2);
    names.push_back("template");
    names.push_back("class");
    names.push_back("demo");
    std::cout << "names size/cap: " << names.size() << "/" << names.capacity() << "\n";
    std::cout << "names[0] = " << names[0] << ", names[2] = " << names[2] << "\n";
    SmallVector<std::string> moved = std::move(names);  // move ctor
    std::cout << "moved size/cap: " << moved.size() << "/" << moved.capacity() << "\n";
    return 0;
}
