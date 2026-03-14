# LeetCode-cpp

## Solved Questions

### Easy

- Q1. Two Sum
- Q20. Valid Parentheses
- Q21. Merge Two Sorted Lists
- Q121. Best Time to Buy and Sell Stock
- Q125. Valid Palindrome
- Q226. Invert Binary Tree
- Q242. Valid Anagram
- Q704. Binary Search
- Q733. Flood Fill
- Q141. Linked List Cycle

### Medium

- _TBD_

### Hard

- _TBD_

## C++ Notes

- [RAII: mechanisms, ideas, and tools](cpp-notes/raii_mechanics_tools.md)
- [Value semantics and object lifetime system](cpp-notes/value_semantics_system.md)
- [Language fundamentals and memory model](cpp-notes/fundamental_and_memory_model.md)
- [Type system and polymorphism](cpp-notes/type_system_and_polymorphism.md)

## Compiler

- [C++ compile and build pipeline](cpp-notes/complier/cpp_build_pipeline.md)
- [From source code to CPU: a compiler-centric view](cpp-notes/complier/compiler_to_cpu_mental_model.md)

## STL

- [Deep understanding of STL](cpp-notes/STL/stl_deep_understanding.md)
- [Toy STL vs standard STL: how far are we from `std::`?](cpp-notes/STL/stl_toy_vs_std.md)

## System

- [gRPC: concepts, architecture, and usage](cpp-notes/system/grpc_overview.md)

### Concurrency Notes

- [Concurrency and systems programming](cpp-notes/currency/concurrency_and_systems.md)
- [Practical concurrency: threads and synchronization](cpp-notes/currency/currency_specific.md)
- [Four layers of concurrency knowledge](cpp-notes/currency/concurrency_layers_overview.md)
- [Classic synchronization problems](cpp-notes/currency/concurrency_classic_problems.md)

### Infra / Performance Components

- **Code & headers**
  - [`MemoryPool` header](codes/includes/memory_pool.h)
  - [`ObjectPool` header](codes/includes/object_pool.h)
  - [`MemoryPoolSafe` header (with debug checks)](codes/includes/memory_pool_safe.h)
  - [`ObjectPoolSafe` header (debug checks + RAII handle)](codes/includes/object_pool_safe.h)
  - [`ThreadPool` header](codes/includes/thread_pool.h)
  - [`SpscRingBuffer` header](codes/includes/spsc_ring_buffer.h)
  - [Example programs and build commands](codes/README.md)

- **Notes**
  - [MemoryPool: small-object pool in practice](cpp-notes/infra/memory_pool.md)
  - [ObjectPool: object lifetime management on a pool](cpp-notes/infra/object_pool.md)
  - [ThreadPool: fixed thread pool and task submission](cpp-notes/infra/thread_pool.md)
  - [SPSC Ring Buffer: single-producer single-consumer queue](cpp-notes/infra/spsc_ring_buffer.md)
  - [MemoryPool vs ObjectPool: design, responsibilities, and safety](cpp-notes/infra/mempool_vs_objectpool_safety.md)

### Data Structures (toy implementations)

- **Code & examples**
  - [`Vector<T>`](codes/includes/vector.h) + [example](codes/src/vector_example.cpp)
  - [`HashTable<K, V>`](codes/includes/hash_table.h) + [example](codes/src/hash_table_example.cpp)
  - [`LRUCache<K, V>`](codes/includes/lru_cache.h) + [example](codes/src/lru_cache_example.cpp)