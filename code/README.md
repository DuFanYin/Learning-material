# C++ 示例代码说明

本目录用于存放 C++ 学习与面试相关示例，按主题分组，统一可通过脚本一键编译。

## 目录结构

- `cpp20/`：C++20 关键能力示例（concepts、ranges、span、jthread、<=> 等）
- `cpp23/`：C++23 关键能力示例（expected、optional monadic、println、if consteval、mdspan 等）
- `mini_stl/`：mini STL 风格容器实现与示例（vector、hash_table、lru_cache）
- `dsa/`：LeetCode 常见算法与数据结构（手写实现版，按“一个主题一个文件”组织）
- `infra/`：基础设施组件（memory_pool、object_pool、spsc、thread_pool）
- `concurrency/`：并发与内存模型示例
- `oop/`：面向对象与多态相关示例
- `perf/`：性能优化相关示例（如 cache alignment）
- `template/`：模板编程示例（函数模板、类模板、可变参数模板、concepts）
- `build_all.sh`：一键编译本目录下全部 `.cpp` 文件

## 一键编译

在仓库根目录执行：

```bash
code/build_all.sh
```

脚本行为：

- 自动递归扫描 `code/**/*.cpp`
- 统一使用 `clang++ -std=c++23`
- 对涉及线程/并发的文件自动加 `-pthread`
- 输出二进制到 `code/build/`

## 单文件编译

示例（在仓库根目录执行）：

```bash
clang++ -std=c++23 -O2 code/cpp23/expected.cpp -o code/build/cpp23_expected
./code/build/cpp23_expected
```

## 使用建议

- 先跑 `code/build_all.sh`，确认环境可用
- 再按主题阅读和运行单个文件，理解注释中的 use case
- 若要新增示例，建议放到对应主题目录并保持命名简洁
