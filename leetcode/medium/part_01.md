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

---

## Q57. Insert Interval

**Question**

You are given an array of non-overlapping intervals `intervals` where `intervals[i] = [start_i, end_i]` are sorted by `start_i`, and a new interval `newInterval`. Insert `newInterval` into `intervals` such that the result is still sorted and non-overlapping (merge overlapping intervals if needed).

**Idea**

- Single pass over sorted intervals in three phases:
  1. **Before:** append intervals that end strictly before `newInterval` starts (`intervals[i][1] < newInterval[0]`).
  2. **Merge:** while intervals overlap the current merged range (`intervals[i][0] <= newInterval[1]`), expand `newInterval` with `min`/`max` of endpoints, then push the merged interval once.
  3. **After:** append the remaining intervals unchanged.

**Code (C++)**

```cpp
class Solution {
public:
    vector<vector<int>> insert(vector<vector<int>>& intervals, vector<int>& newInterval) {
        vector<vector<int>> res;
        int i = 0, n = intervals.size();

        while (i < n && intervals[i][1] < newInterval[0]) {
            res.push_back(intervals[i]);
            i++;
        }

        while (i < n && intervals[i][0] <= newInterval[1]) {
            newInterval[0] = min(newInterval[0], intervals[i][0]);
            newInterval[1] = max(newInterval[1], intervals[i][1]);
            i++;
        }
        res.push_back(newInterval);

        while (i < n) {
            res.push_back(intervals[i]);
            i++;
        }

        return res;
    }
};
```

---

## Q542. 01 Matrix

**Question**

Given an `m x n` binary matrix `mat`, return a matrix of the same size where each cell contains the distance to the nearest `0`. The distance between two adjacent cells (sharing an edge) is `1`.

**Idea**

- **Multi-source BFS:** enqueue all cells with value `0` (distance `0`). Mark all `1` cells as unvisited (e.g. `-1`).
- Expand layer by layer: from each dequeued cell, relax neighbors that are still `-1`, set distance to `current + 1`, and enqueue them.
- First time a cell is reached is the shortest distance (unweighted graph).

**Code (C++)**

```cpp
class Solution {
public:
    vector<vector<int>> updateMatrix(vector<vector<int>>& mat) {
        int m = mat.size(), n = mat[0].size();
        queue<pair<int, int>> q;

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (mat[i][j] == 0) {
                    q.push({i, j});
                } else {
                    mat[i][j] = -1;
                }
            }
        }

        int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();

            for (auto& d : dirs) {
                int nx = x + d[0], ny = y + d[1];

                if (nx >= 0 && nx < m && ny >= 0 && ny < n && mat[nx][ny] == -1) {
                    mat[nx][ny] = mat[x][y] + 1;
                    q.push({nx, ny});
                }
            }
        }

        return mat;
    }
};
```

---

## Q973. K Closest Points to Origin

**Question**

You are given an array of `points` where `points[i] = [x_i, y_i]`. Return the `k` points with the **smallest** Euclidean distance to the origin `(0, 0)`. You may return the points in any order.

**Idea**

- Compare points by squared distance `x^2 + y^2` (avoids floating point).
- Provide **two approaches**:
  - **Approach 1 (Heap / Priority Queue):** maintain a max-heap of size `k`, so the top is the farthest among the selected points.
  - **Approach 2 (Quickselect):** partition the array by distance so the first `k` points are the closest ones.

---

**Code (C++ / Approach 1: Heap)**

```cpp
class Solution {
public:
    vector<vector<int>> kClosest(vector<vector<int>>& points, int k) {
        auto dist = [](const vector<int>& p) -> long long {
            return 1LL * p[0] * p[0] + 1LL * p[1] * p[1];
        };

        // Max-heap by distance; keeps the best k closest.
        priority_queue<pair<long long, int>> pq;
        for (int i = 0; i < (int)points.size(); i++) {
            pq.push({dist(points[i]), i});
            if ((int)pq.size() > k) pq.pop();
        }

        vector<vector<int>> res;
        res.reserve(k);
        while (!pq.empty()) {
            res.push_back(points[pq.top().second]);
            pq.pop();
        }
        return res;
    }
};
```

---

**Code (C++ / Approach 2: Quickselect)**

```cpp
class Solution {
public:
    vector<vector<int>> kClosest(vector<vector<int>>& points, int k) {
        auto dist = [](const vector<int>& p) -> long long {
            return 1LL * p[0] * p[0] + 1LL * p[1] * p[1];
        };

        if (k >= (int)points.size()) return points;

        int left = 0, right = (int)points.size() - 1;
        int target = k - 1; // 0-based index of the k-th closest boundary

        while (left <= right) {
            long long pivotDist = dist(points[right]);
            int i = left;

            for (int j = left; j < right; j++) {
                if (dist(points[j]) <= pivotDist) {
                    swap(points[i], points[j]);
                    i++;
                }
            }
            swap(points[i], points[right]);

            if (i == target) break;
            if (i < target) left = i + 1;
            else right = i - 1;
        }

        return vector<vector<int>>(points.begin(), points.begin() + k);
    }
};
```

---

## Q3. Longest Substring Without Repeating Characters

**Question**

Given a string `s`, find the length of the longest substring without repeating characters.

**Idea**

- Use a sliding window `[left, right]`.
- Maintain the last seen index of each character in an array of size 256.
- When `s[right]` is repeated inside the current window, move `left` to `lastSeen + 1`.
- Update answer with `right - left + 1` at each step.

**Code (C++)**

```cpp
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        int n = (int)s.size();
        if (n == 0) return 0;

        vector<int> lastSeen(256, -1);
        int left = 0;
        int maxLength = 0;

        for (int right = 0; right < n; right++) {
            unsigned char ch = (unsigned char)s[right];
            if (lastSeen[ch] != -1) {
                left = max(left, lastSeen[ch] + 1);
            }

            lastSeen[ch] = right;
            maxLength = max(maxLength, right - left + 1);
        }

        return maxLength;
    }
};
```

---

## Q15. 3Sum

**Question**

Given an integer array `nums`, return all unique triplets `[nums[i], nums[j], nums[k]]` such that `i != j`, `i != k`, `j != k`, and `nums[i] + nums[j] + nums[k] == 0`.

The solution set must not contain duplicate triplets.

**Idea**

- Sort the array first.
- Enumerate the first element `nums[i]`, then use two pointers (`left`, `right`) to find pairs summing to `-nums[i]`.
- Skip duplicates:
  - Skip duplicate `i` values.
  - After finding a valid triplet, move `left`/`right` past equal values.
- If `nums[i] > 0`, break early since later values are also non-negative.

**Code (C++)**

```cpp
class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
        vector<vector<int>> res;
        int n = (int)nums.size();
        sort(nums.begin(), nums.end());

        for (int i = 0; i < n; i++) {
            if (nums[i] > 0) break;
            if (i > 0 && nums[i] == nums[i - 1]) continue;

            int left = i + 1, right = n - 1;
            while (left < right) {
                int sum = nums[i] + nums[left] + nums[right];

                if (sum == 0) {
                    res.push_back({nums[i], nums[left], nums[right]});

                    while (left < right && nums[left] == nums[left + 1]) left++;
                    while (left < right && nums[right] == nums[right - 1]) right--;
                    left++;
                    right--;
                } else if (sum < 0) {
                    left++;
                } else {
                    right--;
                }
            }
        }
        return res;
    }
};
```

---

## Q102. Binary Tree Level Order Traversal

**Question**

Given the `root` of a binary tree, return the level order traversal of its nodes' values (from left to right, level by level).

**Idea**

- Use **BFS** with a queue.
- For each round, process exactly `q.size()` nodes (the current level).
- Collect values into `level`, then push children of each node into the queue.
- Append `level` to the result.

**Code (C++)**

```cpp
class Solution {
public:
    vector<vector<int>> levelOrder(TreeNode* root) {
        vector<vector<int>> res;
        if (!root) return res;

        queue<TreeNode*> q;
        q.push(root);

        while (!q.empty()) {
            int size = q.size();
            vector<int> level;

            for (int i = 0; i < size; i++) {
                TreeNode* node = q.front();
                q.pop();
                level.push_back(node->val);

                if (node->left) q.push(node->left);
                if (node->right) q.push(node->right);
            }

            res.push_back(level);
        }
        return res;
    }
};
```