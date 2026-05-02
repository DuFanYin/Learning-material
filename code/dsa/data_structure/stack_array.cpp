// Compile: clang++ -std=c++23 -O2 code/dsa/data_structure/stack_array.cpp -o code/build/stack_array
// Run:     ./code/build/stack_array
// DSA: stack implementation with fixed array.

#include <iostream>

class StackArray {
public:
    static const int CAP = 1000;

    StackArray() : top_(-1) {}

    bool empty() const { return top_ == -1; }
    int size() const { return top_ + 1; }

    bool push(int x) {
        if (top_ + 1 >= CAP) return false;
        a_[++top_] = x;
        return true;
    }

    bool pop() {
        if (empty()) return false;
        --top_;
        return true;
    }

    int top() const { return empty() ? -1 : a_[top_]; }

private:
    int a_[CAP];
    int top_;
};

int main() {
    StackArray st;
    st.push(10);
    st.push(20);
    st.push(30);
    std::cout << "top = " << st.top() << ", size = " << st.size() << "\n";
    st.pop();
    std::cout << "top after pop = " << st.top() << "\n";
    return 0;
}
