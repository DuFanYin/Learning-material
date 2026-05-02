## Q1. Two Sum

**Question**

Given an array `nums` and a target value `target`, find two different indices `i` and `j` such that:

`nums[i] + nums[j] = target`

Return the indices of these two numbers.

**Idea**

- **Use a hash map** (`unordered_map`) to store numbers you have already seen while iterating through the array.
- For each number `nums[i]`, compute the complement needed to reach the target:
  - `complement = target - nums[i]`
- Check if this complement has already appeared earlier in the map.

**Code (C++)**

```cpp
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> numMap;
        for (int i = 0; i < nums.size(); i++) {
            int complement = target - nums[i];
            if (numMap.find(complement) != numMap.end()) {
                return {numMap[complement], i};
            }
            numMap[nums[i]] = i;
        }
        return {};
    }
};
```

---

## Q20. Valid Parentheses

**Question**

Given a string `s` containing just the characters `'('`, `')'`, `'{'`, `'}'`, `'['` and `']'`, determine if the input string is valid.

An input string is valid if:

- **Open brackets must be closed by the same type of brackets.**
- **Open brackets must be closed in the correct order.**
- **Every close bracket has a corresponding open bracket of the same type.**

**Idea**

- **Use a stack** to keep track of open brackets.
- Traverse the string:
  - Push open brackets onto the stack.
  - For a closing bracket, check the top of the stack for the matching opening bracket.
  - If it doesn’t match or the stack is empty, return `false`.
- At the end, the stack must be empty for the string to be valid.

**Code (C++)**

```cpp
class Solution {
public:
    bool isValid(string s) {
        stack<char> st;
        for (char ch : s) {
            if (ch == '(' || ch == '[' || ch == '{') {
                st.push(ch);
            } else {
                if (st.empty()) {
                    return false;
                }
                char top = st.top();
                st.pop();
                if (ch == ')' && top != '(') return false;
                if (ch == ']' && top != '[') return false;
                if (ch == '}' && top != '{') return false;
            }
        }
        return st.empty();
    }
};
```

---

## Q21. Merge Two Sorted Lists

**Question**

Merge two sorted linked lists and return the head of the merged sorted linked list.

**Idea**

- **Use a dummy and a temp pointer** to build the new list.
- While both `list1` and `list2` are not null:
  - Compare `list1->val` and `list2->val`.
  - Attach the smaller node to `temp->next`, and move that list’s pointer forward.
  - Move `temp` forward to `temp->next`.
- After the loop, **attach the remaining nodes** of the non-empty list (either `list1` or `list2`).
- Return `dummy->next` as the merged list head.

**Code (C++)**

```cpp
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode* dummy = new ListNode();
        ListNode* temp = dummy;

        while (list1 && list2) {
            if (list1->val < list2->val) {
                temp->next = list1;
                list1 = list1->next;
            } else {
                temp->next = list2;
                list2 = list2->next;
            }
            temp = temp->next;
        }

        if (list1) temp->next = list1;
        if (list2) temp->next = list2;

        return dummy->next;
    }
};
```

---

## Q121. Best Time to Buy and Sell Stock

**Question**

Given an array `prices` where `prices[i]` is the price of a given stock on the \(i\)-th day, find the maximum profit you can achieve from a single buy and a single sell. You must buy before you sell. If you cannot achieve any profit, return `0`.

**Idea**

- **Track the minimum price so far** (`buy`) as you iterate through the array.
- For each day \(i\):
  - Update `buy` if `prices[i]` is smaller (better buying price).
  - Otherwise, compute the profit `prices[i] - buy` and update `profit` if it is larger than the current maximum.
- The final `profit` is the answer (or `0` if prices only go down).

**Code (C++)**

```cpp
class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int buy = prices[0];
        int profit = 0;

        for (int i = 1; i < prices.size(); i++) {
            if (prices[i] < buy) {
                buy = prices[i];
            } else {
                profit = max(profit, prices[i] - buy);
            }
        }

        return profit;
    }
};
```

---

## 125. Valid Palindrome

**Question**

Given a string `s`, determine if it is a palindrome, considering only alphanumeric characters and ignoring cases.

**Idea**

- First **clean** the string:
  - Keep only alphanumeric characters.
  - Convert everything to lowercase so that `'A'` and `'a'` are treated the same.
- Then use **two pointers** from the start and end of the cleaned string and compare characters until they meet.

**Code (C++)**

```cpp
class Solution {
public:
    bool isPalindrome(string s) {
        string cleaned = "";
        for (char ch : s){
            if (isalnum(ch)){
                cleaned += tolower(ch);
            }
        }

        int start = 0;
        int end = cleaned.length() - 1;

        while (start <= end){
            if (cleaned[start] != cleaned[end]){
                return false;
            }

            start++;
            end--;
        }

        return true;
    }
};
```

---

## 226. Invert Binary Tree

**Question**

Invert a binary tree: swap the left and right subtree for every node.

**Idea**

- Use recursion:
  - For each node, swap `left` and `right`.
  - Then recursively invert the left and right subtrees.

**Code (C++)**

```cpp
class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {
        if (root){
            TreeNode* temp = root -> left;
            root->left = root->right;
            root->right = temp;
            invertTree(root->left);
            invertTree(root->right);
        }
        return root;
    }
};
```

---

## 242. Valid Anagram

**Question**

Given two strings `s` and `t`, check whether `t` is an anagram of `s`.

**Idea**

- Use a single length-26 frequency array to track letter counts:
  - For each position `i`, increment the count for `s[i]` and decrement for `t[i]`.
  - At the end, if all counts are zero, the two strings are anagrams.

**Code (C++)**

```cpp
class Solution {
public:
    bool isAnagram(string s, string t) {
        if (s.length() != t.length()) {
            return false;
        }
        
        vector<int> freq(26, 0);

        for (int i=0 ; i < s.length() ; i++){
            freq[s[i] - 'a']++;
            freq[t[i] - 'a']--;
        }

        for (int i=0; i < freq.size(); i++){
            if (freq[i] != 0){
                return false;
            }
        }

        return true;
    }
};
```

---

## 704. Binary Search

**Question**

Find the index of `target` in a sorted array `nums` (or return `-1` if not found).

**Idea**

- Apply standard binary search on the sorted array using `low`, `high`, and `mid`.

**Code (C++)**

```cpp
class Solution {
public:
    int search(vector<int>& nums, int target) {
        int low = 0;
        int high = nums.size() - 1;

        while(low <= high){
            int mid = low + (high - low) / 2;
            if(nums[mid] == target){
                return mid;
            }
            else if(nums[mid] < target){
                low = mid + 1;
            }
            else{
                high = mid - 1;
            }
        }
        return -1;
    }
};
```

---

## 733. Flood Fill

**Question**

Given a 2D image represented by a grid of integers, a starting pixel \((sr, sc)\), and a new color `color`, fill the entire connected region (4-directionally) of the starting pixel with the new color.

**Idea**

- Use **depth-first search (DFS)** from the starting cell.
- The problem allows us to modify `image` in-place, so we don't need an extra `visited` structure.
- Recurse to neighbors only if they are in bounds and have the same original color.

**Code (C++)**

```cpp
class Solution {
public:
    void dfs(vector<vector<int>>& image, int i, int j, int val, int newColor) {
        if (i < 0 || i >= image.size() || j < 0 || j >= image[0].size() || image[i][j] == newColor || image[i][j] != val) {
            return;
        }

        image[i][j] = newColor;
        dfs(image, i - 1, j, val, newColor);
        dfs(image, i + 1, j, val, newColor);
        dfs(image, i, j - 1, val, newColor);
        dfs(image, i, j + 1, val, newColor);
    }

    vector<vector<int>> floodFill(vector<vector<int>>& image, int sr, int sc, int color) {
        int val = image[sr][sc];
        dfs(image, sr, sc, val, color);
        return image;
    }
};
```

---

## 141. Linked List Cycle

**Question**

Given the head of a linked list, determine if the list has a cycle in it. Return `true` if there is a cycle, and `false` otherwise. You do not need to find the node where the cycle begins.

**Idea**

- Use **two pointers**: a slow pointer that moves one step at a time and a fast pointer that moves two steps.
- If there is a cycle, the fast pointer will eventually meet the slow pointer.
- If the fast pointer reaches `NULL` (the end), then there is no cycle.

**Code (C++)**

```cpp
class Solution {
public:
    bool hasCycle(ListNode *head) {
        if (head == NULL || head->next == NULL) {
            return false;
        }

        ListNode* slow = head;
        ListNode* fast = head->next;

        while (fast != slow) {
            if (fast->next == NULL || fast->next->next == NULL) {
                return false;
            }

            fast = fast->next->next;
            slow = slow->next;
        }
        return true;
    }
};
```