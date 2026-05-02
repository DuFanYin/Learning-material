## Q110. Balanced Binary Tree

**Question**

Given the root of a binary tree, determine whether it is height-balanced.

A binary tree is height-balanced if, for every node, the height difference between its left and right subtrees is at most 1.

**Idea**

- Use a **bottom-up DFS** to compute subtree heights.
- Return a sentinel value `-1` as soon as an unbalanced subtree is found.
- For each node:
  - Recursively get `leftHeight` and `rightHeight`.
  - If either side is `-1`, propagate `-1` upward.
  - If `abs(leftHeight - rightHeight) > 1`, return `-1`.
  - Otherwise return `1 + max(leftHeight, rightHeight)`.
- The tree is balanced iff the final height is not `-1`.

**Code (C++)**

```cpp
class Solution {
public:
    bool isBalanced(TreeNode* root) {
        return dfsHeight(root) != -1;
    }

private:
    int dfsHeight(TreeNode* node) {
        if (node == nullptr) return 0;

        int leftHeight = dfsHeight(node->left);
        if (leftHeight == -1) return -1;

        int rightHeight = dfsHeight(node->right);
        if (rightHeight == -1) return -1;

        if (abs(leftHeight - rightHeight) > 1) return -1;

        return 1 + max(leftHeight, rightHeight);
    }
};
```

---

## Q232. Implement Queue using Stacks

**Question**

Implement a first-in-first-out (FIFO) queue using only two stacks. Support `push`, `pop`, `peek`, and `empty`.

**Idea**

- Maintain two stacks:
  - `input` for incoming elements.
  - `output` for elements in queue order.
- `push(x)` always pushes into `input`.
- For `peek()` / `pop()`:
  - If `output` is empty, move all elements from `input` to `output`.
  - This reversal makes the oldest element appear at the top of `output`.
- `empty()` is true only when both stacks are empty.

**Code (C++)**

```cpp
class MyQueue {
private:
    stack<int> input;
    stack<int> output;

public:
    MyQueue() {}

    void push(int x) {
        input.push(x);
    }

    int pop() {
        int val = peek();
        output.pop();
        return val;
    }

    int peek() {
        if (output.empty()) {
            while (!input.empty()) {
                output.push(input.top());
                input.pop();
            }
        }
        return output.top();
    }

    bool empty() {
        return input.empty() && output.empty();
    }
};
```

---

## Q235. Lowest Common Ancestor of a Binary Search Tree

**Question**

Given a binary search tree (BST) and two nodes `p` and `q`, return their lowest common ancestor (LCA).

**Idea**

- Use the BST ordering property:
  - If both `p` and `q` are smaller than `root`, move to `root->left`.
  - If both are larger than `root`, move to `root->right`.
  - Otherwise, current `root` is the split point and is the LCA.
- This gives an iterative `O(h)` solution, where `h` is tree height.

**Code (C++)**

```cpp
class Solution {
public:
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        while (root) {
            if (p->val < root->val && q->val < root->val) {
                root = root->left;
            } else if (p->val > root->val && q->val > root->val) {
                root = root->right;
            } else {
                return root;
            }
        }
        return nullptr;
    }
};
```

---

## Q278. First Bad Version

**Question**

You have `n` versions `[1, 2, ..., n]` and want to find the first bad one. `isBadVersion(version)` returns whether that version is bad. Minimize calls to `isBadVersion`.

**Idea**

- **Binary search** on the answer range `[1, n]`.
- If `isBadVersion(mid)` is true, the first bad version is at `mid` or to the left: `right = mid - 1`.
- Otherwise the first bad is to the right: `left = mid + 1`.
- When the loop ends, `left` is the first bad version.

**Code (C++)**

```cpp
class Solution {
public:
    int firstBadVersion(int n) {
        int left = 0, right = n;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (isBadVersion(mid)) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return left;
    }
};
```

---

## Q383. Ransom Note

**Question**

Given two strings `ransomNote` and `magazine`, return true if `ransomNote` can be built using letters from `magazine`, where each letter in `magazine` may be used at most once.

**Idea**

- Count characters in `magazine` with a hash map (or fixed array for lowercase English).
- For each character in `ransomNote`, decrement the count; if any character is missing or exhausted, return false.

**Code (C++)**

```cpp
class Solution {
public:
    bool canConstruct(string ransomNote, string magazine) {
        unordered_map<char, int> dictionary;

        for (char c : magazine) {
            if (dictionary.find(c) != dictionary.end()) {
                dictionary[c]++;
            } else {
                dictionary[c] = 1;
            }
        }

        for (char c : ransomNote) {
            if (dictionary.find(c) != dictionary.end() && dictionary[c] > 0) {
                dictionary[c]--;
            } else {
                return false;
            }
        }

        return true;
    }
};
```

---

## Q70. Climbing Stairs

**Question**

You are climbing a staircase. It takes `n` steps to reach the top.

Each time you can climb either `1` or `2` steps. In how many distinct ways can you climb to the top?

**Idea**

- Let `f(n)` be the number of ways to reach step `n`. Then `f(n) = f(n-1) + f(n-2)` (last move is 1 or 2 steps).
- Base cases: `f(1) = 1`, `f(2) = 2` (or treat `n < 3` as in the code below).
- Use **memoization** (hash map) to avoid recomputing the same subproblems in the recursive formulation.

**Code (C++)**

```cpp
class Solution {
public:
    unordered_map<int, int> umap;

    int climbStairs(int n) {
        if (umap.contains(n)) {
            return umap[n];
        }
        int result;
        if (n < 3) {
            result = n;
        } else {
            result = climbStairs(n - 1) + climbStairs(n - 2);
        }
        umap[n] = result;
        return result;
    }
};
```

---

## Q409. Longest Palindrome

**Question**

Given a string `s` consisting of uppercase and/or lowercase letters, return the length of the longest palindrome that can be built with those letters (letters may be rearranged; each letter is used at most as many times as it appears in `s`).

**Idea**

- Count each character’s frequency with a hash map.
- For each frequency: add all of it if it is even; if odd, add `freq - 1` (the largest even part) and remember that this character had an odd count (can supply a middle letter).
- If any character had an odd frequency, add `1` for the center of the palindrome.

**Code (C++)**

```cpp
class Solution {
public:
    int longestPalindrome(string s) {
        unordered_map<char, int> count;

        for (char c : s) {
            count[c]++;
        }

        int length = 0;
        bool hasOdd = false;

        for (auto &pair : count) {
            int freq = pair.second;
            if (freq % 2 == 0) {
                length += freq;
            } else {
                length += freq - 1;
                hasOdd = true;
            }
        }

        if (hasOdd) {
            length += 1;
        }

        return length;
    }
};
```

---

## Q206. Reverse Linked List

**Question**

Given the `head` of a singly linked list, reverse the list and return the new head.

**Idea**

- **Iterative reversal:** maintain `prev` (new head-so-far) and `current` (node being processed).
- Save `current->next`, point `current->next` backward to `prev`, then advance `prev` and `current`.
- When `current` is null, `prev` is the head of the reversed list.

**Code (C++)**

```cpp
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode* prev = nullptr;
        ListNode* current = head;
        ListNode* next = nullptr;

        while (current != nullptr) {
            next = current->next;
            current->next = prev;
            prev = current;
            current = next;
        }
        return prev;
    }
};
```

---

## Q169. Majority Element

**Question**

Given an array `nums` of size `n`, return the **majority element**—the value that appears strictly more than `⌊n / 2⌋` times. You may assume the majority element always exists.

**Idea**

- **Boyer–Moore voting:** treat the majority as “winning” pairwise cancellations against all other values.
- Maintain a `candidate` and a `count`. When `count` is 0, pick the current element as the new `candidate`.
- For each element, increment `count` if it matches `candidate`, else decrement. The majority survives as the final `candidate`.

**Code (C++)**

```cpp
class Solution {
public:
    int majorityElement(vector<int>& nums) {
        int count = 0;
        int candidate = 0;

        for (int n : nums) {
            if (count == 0) {
                candidate = n;
            }
            count += (n == candidate) ? 1 : -1;
        }

        return candidate;
    }
};
```

---

## Q67. Add Binary

**Question**

Given two binary strings `a` and `b`, return their sum as a binary string.

**Idea**

- Simulate **column-by-column addition** from the least significant bit (right end of each string).
- Track `carry`; each step adds bits from `a` and `b` if available, append `(sum % 2)` to the result, then set `carry = sum / 2`.
- After the loop, **reverse** the built string (or prepend—here append then `reverse`).

**Code (C++)**

```cpp
class Solution {
public:
    string addBinary(string a, string b) {
        string result = "";
        int i = a.size() - 1;
        int j = b.size() - 1;
        int carry = 0;

        while (i >= 0 || j >= 0 || carry) {
            int sum = carry;

            if (i >= 0) sum += a[i--] - '0';
            if (j >= 0) sum += b[j--] - '0';

            result += (sum % 2) + '0';
            carry = sum / 2;
        }

        reverse(result.begin(), result.end());
        return result;
    }
};
```