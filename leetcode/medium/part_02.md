## Q207. Course Schedule

**Question**

There are `numCourses` courses labeled from `0` to `numCourses - 1`.
You are given `prerequisites`, where `prerequisites[i] = [a, b]` means you must take course `b` before course `a`.

Return `true` if you can finish all courses, otherwise return `false`.

**Idea**

- Model courses as a directed graph.
- Use indegree counting + BFS topological sort (Kahn's algorithm).
- Push all nodes with indegree `0` into a queue.
- If the number of processed courses equals `numCourses`, there is no cycle.

**Code (C++)**

```cpp
class Solution {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> graph(numCourses);
        vector<int> indegree(numCourses, 0);

        // Build graph.
        for (auto& p : prerequisites) {
            int a = p[0], b = p[1];
            graph[b].push_back(a);
            indegree[a]++;
        }

        queue<int> q;

        // Push nodes with indegree 0.
        for (int i = 0; i < numCourses; i++) {
            if (indegree[i] == 0) q.push(i);
        }

        int count = 0;

        while (!q.empty()) {
            int cur = q.front();
            q.pop();
            count++;

            for (int next : graph[cur]) {
                if (--indegree[next] == 0) {
                    q.push(next);
                }
            }
        }

        return count == numCourses;
    }
};
```

---

## Q200. Number of Islands

**Question**

Given a 2D grid of `'1'` (land) and `'0'` (water), count the number of islands.
An island is formed by connecting adjacent lands horizontally or vertically.

**Idea**

- Iterate through every cell in the grid.
- When a `'1'` is found, increment island count and run DFS to flood-fill that island.
- During DFS, mark visited land as `'0'` to avoid recounting.

**Code (C++)**

```cpp
class Solution {
public:
    void dfs(vector<vector<char>>& grid, int i, int j) {
        int m = grid.size(), n = grid[0].size();

        // Out of bounds or water.
        if (i < 0 || j < 0 || i >= m || j >= n || grid[i][j] == '0') return;

        grid[i][j] = '0'; // Mark as visited.

        dfs(grid, i + 1, j);
        dfs(grid, i - 1, j);
        dfs(grid, i, j + 1);
        dfs(grid, i, j - 1);
    }

    int numIslands(vector<vector<char>>& grid) {
        int m = grid.size(), n = grid[0].size();
        int count = 0;

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == '1') {
                    count++;
                    dfs(grid, i, j);
                }
            }
        }

        return count;
    }
};
```

---

## Q322. Coin Change

**Question**

Given an array of coin denominations `coins` and an integer `amount`,
return the fewest number of coins needed to make up that amount.
If it cannot be formed, return `-1`.

**Idea**

- Use bottom-up dynamic programming.
- Let `dp[i]` be the minimum number of coins needed to form amount `i`.
- Transition: `dp[i] = min(dp[i], dp[i - coin] + 1)` for all valid `coin`.
- Initialize `dp[0] = 0`, others as unreachable (`INT_MAX`).

**Code (C++)**

```cpp
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        vector<int> dp(amount + 1, INT_MAX);
        dp[0] = 0;

        for (int i = 1; i <= amount; i++) {
            for (int coin : coins) {
                if (i - coin >= 0 && dp[i - coin] != INT_MAX) {
                    dp[i] = min(dp[i], dp[i - coin] + 1);
                }
            }
        }

        return dp[amount] == INT_MAX ? -1 : dp[amount];
    }
};
```

---

## Q994. Rotting Oranges

**Question**

You are given an `m x n` grid where:
- `0` = empty cell
- `1` = fresh orange
- `2` = rotten orange

Every minute, any fresh orange adjacent (up, down, left, right) to a rotten orange becomes rotten.
Return the minimum number of minutes needed so that no fresh orange remains. If impossible, return `-1`.

**Idea**

- Use multi-source BFS starting from all initially rotten oranges.
- Each BFS layer represents one minute.
- Track the number of fresh oranges; decrement when one becomes rotten.
- When BFS ends, return minutes if `fresh == 0`, otherwise `-1`.

**Code (C++)**

```cpp
class Solution {
public:
    int orangesRotting(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        queue<pair<int, int>> q;
        int fresh = 0;

        // Initialize queue with all rotten oranges.
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 2) q.push({i, j});
                if (grid[i][j] == 1) fresh++;
            }
        }

        int minutes = 0;
        int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

        // BFS by layers (each layer = 1 minute).
        while (!q.empty() && fresh > 0) {
            int size = q.size();
            minutes++;

            for (int i = 0; i < size; i++) {
                auto [x, y] = q.front();
                q.pop();

                for (auto& d : dirs) {
                    int nx = x + d[0], ny = y + d[1];
                    if (nx >= 0 && ny >= 0 && nx < m && ny < n && grid[nx][ny] == 1) {
                        grid[nx][ny] = 2;
                        q.push({nx, ny});
                        fresh--;
                    }
                }
            }
        }

        return fresh == 0 ? minutes : -1;
    }
};
```