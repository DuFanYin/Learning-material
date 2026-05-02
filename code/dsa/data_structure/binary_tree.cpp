// Compile: clang++ -std=c++23 -O2 code/dsa/data_structure/binary_tree.cpp -o code/build/binary_tree
// Run:     ./code/build/binary_tree
// LeetCode: actual reusable BinaryTree class.

#include <algorithm>
#include <iostream>
#include <optional>
#include <queue>
#include <vector>

class BinaryTree {
public:
    struct Node {
        int val;
        Node* left;
        Node* right;
        Node(int v) : val(v), left(nullptr), right(nullptr) {}
    };

    BinaryTree() : root_(nullptr) {}
    ~BinaryTree() { clear(root_); }

    // Build complete binary tree from array:
    // index i -> left 2*i+1, right 2*i+2
    void build_complete(const std::vector<int>& arr) {
        clear(root_);
        if (arr.empty()) {
            root_ = nullptr;
            return;
        }

        std::vector<Node*> nodes;
        nodes.reserve(arr.size());
        for (int x : arr) {
            nodes.push_back(new Node(x));
        }
        for (int i = 0; i < nodes.size(); ++i) {
            int l = i * 2 + 1;
            int r = i * 2 + 2;
            if (l < nodes.size()) {
                nodes[i]->left = nodes[l];
            }
            if (r < nodes.size()) {
                nodes[i]->right = nodes[r];
            }
        }
        root_ = nodes[0];
    }

    // Build from LeetCode-style level order input with nulls.
    // Example: {1,2,3,null,5,6,7}
    void build_level_order(const std::vector<std::optional<int>>& vals) {
        clear(root_);
        root_ = nullptr;
        if (vals.empty() || !vals[0].has_value()) {
            return;
        }

        root_ = new Node(*vals[0]);
        std::queue<Node*> q;
        q.push(root_);
        int i = 1;

        while (!q.empty() && i < vals.size()) {
            Node* cur = q.front();
            q.pop();

            if (i < vals.size() && vals[i].has_value()) {
                cur->left = new Node(*vals[i]);
                q.push(cur->left);
            }
            ++i;

            if (i < vals.size() && vals[i].has_value()) {
                cur->right = new Node(*vals[i]);
                q.push(cur->right);
            }
            ++i;
        }
    }

    int height() const { return height_dfs(root_); }

    std::vector<int> preorder() const {
        std::vector<int> out;
        preorder_dfs(root_, out);
        return out;
    }

    std::vector<int> inorder() const {
        std::vector<int> out;
        inorder_dfs(root_, out);
        return out;
    }

    std::vector<int> postorder() const {
        std::vector<int> out;
        postorder_dfs(root_, out);
        return out;
    }

    std::vector<int> levelorder() const {
        std::vector<int> out;
        if (!root_) {
            return out;
        }
        std::queue<Node*> q;
        q.push(root_);
        while (!q.empty()) {
            Node* cur = q.front();
            q.pop();
            out.push_back(cur->val);
            if (cur->left) q.push(cur->left);
            if (cur->right) q.push(cur->right);
        }
        return out;
    }

private:
    Node* root_;

    void clear(Node* root) {
        if (!root) {
            return;
        }
        clear(root->left);
        clear(root->right);
        delete root;
    }

    static int height_dfs(Node* root) {
        if (!root) {
            return 0;
        }
        return 1 + std::max(height_dfs(root->left), height_dfs(root->right));
    }

    static void preorder_dfs(Node* root, std::vector<int>& out) {
        if (!root) {
            return;
        }
        out.push_back(root->val);
        preorder_dfs(root->left, out);
        preorder_dfs(root->right, out);
    }

    static void inorder_dfs(Node* root, std::vector<int>& out) {
        if (!root) {
            return;
        }
        inorder_dfs(root->left, out);
        out.push_back(root->val);
        inorder_dfs(root->right, out);
    }

    static void postorder_dfs(Node* root, std::vector<int>& out) {
        if (!root) {
            return;
        }
        postorder_dfs(root->left, out);
        postorder_dfs(root->right, out);
        out.push_back(root->val);
    }
};

void print_vec(const std::vector<int>& v) {
    for (int x : v) std::cout << x << ' ';
    std::cout << "\n";
}

int main() {
    BinaryTree tree;
    tree.build_complete({1, 2, 3, 4, 5, 6, 7});

    std::cout << "preorder: ";
    print_vec(tree.preorder());
    std::cout << "inorder: ";
    print_vec(tree.inorder());
    std::cout << "postorder: ";
    print_vec(tree.postorder());
    std::cout << "levelorder: ";
    print_vec(tree.levelorder());
    std::cout << "height = " << tree.height() << "\n";

    BinaryTree lc_tree;
    lc_tree.build_level_order({1, 2, 3, std::nullopt, 5, 6, 7});
    std::cout << "lc levelorder: ";
    print_vec(lc_tree.levelorder());
    std::cout << "lc height = " << lc_tree.height() << "\n";
    return 0;
}
