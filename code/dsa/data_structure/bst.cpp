// Compile: clang++ -std=c++23 -O2 code/dsa/data_structure/bst.cpp -o code/build/bst
// Run:     ./code/build/bst
// LeetCode: Binary Search Tree class with common operations.

#include <iostream>
#include <vector>

class BST {
public:
    struct Node {
        int val;
        Node* left;
        Node* right;
        Node(int v) : val(v), left(nullptr), right(nullptr) {}
    };

    BST() : root_(nullptr) {}
    ~BST() { clear(root_); }

    void insert(int x) { root_ = insert(root_, x); }

    bool contains(int x) const { return contains(root_, x); }

    void erase(int x) { root_ = erase(root_, x); }

    int min_value() const {
        Node* p = root_;
        while (p && p->left) p = p->left;
        return p ? p->val : -1;
    }

    int max_value() const {
        Node* p = root_;
        while (p && p->right) p = p->right;
        return p ? p->val : -1;
    }

    std::vector<int> inorder() const {
        std::vector<int> out;
        inorder(root_, out);
        return out;
    }

private:
    Node* root_;

    static Node* insert(Node* root, int x) {
        if (!root) return new Node(x);
        if (x < root->val) root->left = insert(root->left, x);
        else if (x > root->val) root->right = insert(root->right, x);
        return root;
    }

    static bool contains(Node* root, int x) {
        while (root) {
            if (x == root->val) return true;
            root = (x < root->val) ? root->left : root->right;
        }
        return false;
    }

    static Node* erase(Node* root, int x) {
        if (!root) return nullptr;
        if (x < root->val) {
            root->left = erase(root->left, x);
        } else if (x > root->val) {
            root->right = erase(root->right, x);
        } else {
            if (!root->left) {
                Node* r = root->right;
                delete root;
                return r;
            }
            if (!root->right) {
                Node* l = root->left;
                delete root;
                return l;
            }
            Node* s = root->right;
            while (s->left) s = s->left;
            root->val = s->val;
            root->right = erase(root->right, s->val);
        }
        return root;
    }

    static void inorder(Node* root, std::vector<int>& out) {
        if (!root) return;
        inorder(root->left, out);
        out.push_back(root->val);
        inorder(root->right, out);
    }

    static void clear(Node* root) {
        if (!root) return;
        clear(root->left);
        clear(root->right);
        delete root;
    }
};

void print_vec(const std::vector<int>& v) {
    for (int x : v) std::cout << x << ' ';
    std::cout << "\n";
}

int main() {
    BST bst;
    for (int x : {8, 3, 10, 1, 6, 14, 4, 7, 13}) {
        bst.insert(x);
    }

    std::cout << "inorder: ";
    print_vec(bst.inorder());
    std::cout << "contains 7: " << std::boolalpha << bst.contains(7) << "\n";
    std::cout << "min/max: " << bst.min_value() << "/" << bst.max_value() << "\n";

    bst.erase(3);
    std::cout << "after erase 3: ";
    print_vec(bst.inorder());
    return 0;
}
