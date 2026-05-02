// Compile: clang++ -std=c++23 -O2 code/dsa/data_structure/linked_list.cpp -o code/build/linked_list
// Run:     ./code/build/linked_list
// LeetCode: actual reusable LinkedList class with core operations.

#include <iostream>

class LinkedList {
public:
    struct Node {
        int val;
        Node* next;
        Node(int v) : val(v), next(nullptr) {}
    };

    LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    ~LinkedList() { clear(); }

    void push_front(int x) {
        Node* node = new Node(x);
        node->next = head_;
        head_ = node;
        if (!tail_) {
            tail_ = node;
        }
        ++size_;
    }

    void push_back(int x) {
        Node* node = new Node(x);
        if (!head_) {
            head_ = tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    void reverse() {
        Node* prev = nullptr;
        Node* cur = head_;
        tail_ = head_;
        while (cur) {
            Node* nxt = cur->next;
            cur->next = prev;
            prev = cur;
            cur = nxt;
        }
        head_ = prev;
    }

    int size() const { return size_; }

    void print() const {
        Node* cur = head_;
        while (cur) {
            std::cout << cur->val << (cur->next ? " -> " : "\n");
            cur = cur->next;
        }
    }

    void clear() {
        Node* cur = head_;
        while (cur) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

private:
    Node* head_;
    Node* tail_;
    int size_;
};

int main() {
    LinkedList list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_front(0);

    std::cout << "list: ";
    list.print();
    std::cout << "size = " << list.size() << "\n";

    list.reverse();
    std::cout << "reversed: ";
    list.print();
    return 0;
}
