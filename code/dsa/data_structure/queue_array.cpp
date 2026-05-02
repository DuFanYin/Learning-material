// Compile: clang++ -std=c++23 -O2 code/dsa/data_structure/queue_array.cpp -o code/build/queue_array
// Run:     ./code/build/queue_array
// DSA: queue implementation with circular array.

#include <iostream>

class QueueArray {
public:
    static const int CAP = 1000;

    QueueArray() : head_(0), tail_(0), sz_(0) {}

    bool empty() const { return sz_ == 0; }
    int size() const { return sz_; }

    bool push(int x) {
        if (sz_ == CAP) return false;
        a_[tail_] = x;
        tail_ = (tail_ + 1) % CAP;
        ++sz_;
        return true;
    }

    bool pop() {
        if (empty()) return false;
        head_ = (head_ + 1) % CAP;
        --sz_;
        return true;
    }

    int front() const { return empty() ? -1 : a_[head_]; }

private:
    int a_[CAP];
    int head_, tail_, sz_;
};

int main() {
    QueueArray q;
    q.push(1);
    q.push(2);
    q.push(3);
    std::cout << "front = " << q.front() << ", size = " << q.size() << "\n";
    q.pop();
    std::cout << "front after pop = " << q.front() << "\n";
    return 0;
}
