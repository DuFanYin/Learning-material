## Q543. Diameter of Binary Tree

**Question**

Given the `root` of a binary tree, return the length of the tree's diameter.

The diameter is the length (number of edges) of the longest path between any two nodes in the tree.

**Idea**

- Use DFS to compute each node's subtree height.
- At each node, a path passing through it has length `leftHeight + rightHeight`.
- Keep a global maximum `ans` for that value.
- Return `1 + max(leftHeight, rightHeight)` as the current node's height.

**Code (C++)**

```cpp
class Solution {
public:
    int ans = 0;

    int dfs(TreeNode* node) {
        if (!node) return 0;

        int left = dfs(node->left);
        int right = dfs(node->right);
        ans = max(ans, left + right);

        return max(left, right) + 1;
    }

    int diameterOfBinaryTree(TreeNode* root) {
        dfs(root);
        return ans;
    }
};
```

---

## Q876. Middle of the Linked List

**Question**

Given the `head` of a singly linked list, return the middle node.

If there are two middle nodes, return the second middle node.

**Idea**

- Use two pointers:
  - `slow` moves one step each iteration.
  - `fast` moves two steps each iteration.
- When `fast` reaches the end, `slow` is at the middle.
- This naturally returns the second middle in even-length lists.

**Code (C++)**

```cpp
class Solution {
public:
    ListNode* middleNode(ListNode* head) {
        ListNode* slow = head;
        ListNode* fast = head;

        while (fast != nullptr && fast->next != nullptr) {
            slow = slow->next;
            fast = fast->next->next;
        }

        return slow;
    }
};
```

---

## Q104. Maximum Depth of Binary Tree

**Question**

Given the `root` of a binary tree, return its maximum depth.

**Idea**

- Use recursive DFS.
- Base case: if `root == nullptr`, depth is `0`.
- Otherwise depth is `1 + max(depth(left), depth(right))`.

**Code (C++)**

```cpp
class Solution {
public:
    int maxDepth(TreeNode* root) {
        if (root == nullptr) return 0;
        return 1 + max(maxDepth(root->left), maxDepth(root->right));
    }
};
```

---

## Q217. Contains Duplicate

**Question**

Given an integer array `nums`, return `true` if any value appears at least twice, and return `false` if every element is distinct.

**Idea**

- Use an `unordered_set<int>` to record seen numbers.
- If a number is already in the set, return `true`.
- If the loop ends without repetition, return `false`.

**Code (C++)**

```cpp
class Solution {
public:
    bool containsDuplicate(vector<int>& nums) {
        unordered_set<int> s;

        for (int n : nums) {
            if (s.count(n)) return true;
            s.insert(n);
        }

        return false;
    }
};
```

---

## Q53. Maximum Subarray

**Question**

Given an integer array `nums`, find the contiguous subarray with the largest sum and return that sum.

**Idea**

- **Kadane’s algorithm:** scan left to right while maintaining the best sum ending at the current index.
- Let `cur` be the maximum sum of a subarray that ends at `i`. Either extend the previous subarray (`cur + nums[i]`) or start fresh at `nums[i]` — take `max(nums[i], cur + nums[i])`.
- Track `best` as the maximum `cur` seen so far.

**Code (C++)**

```cpp
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int cur = nums[0];
        int best = nums[0];

        for (int i = 1; i < (int)nums.size(); i++) {
            cur = max(nums[i], cur + nums[i]);
            best = max(best, cur);
        }

        return best;
    }
};
```
