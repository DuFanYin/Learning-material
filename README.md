# Learning Materials

A growing C++ and systems-programming notebook: LeetCode solutions, deeper notes, and small runnable experiments that evolve over time as I learn.

## C++ Notes

- [RAII: mechanisms, ideas, and tools](cpp-notes/raii_mechanics_tools.md)
- [Value semantics and object lifetime system](cpp-notes/value_semantics_system.md)
- [Language fundamentals and memory model](cpp-notes/fundamental_and_memory_model.md)
- [Type system and polymorphism](cpp-notes/type_system_and_polymorphism.md)

### Compiler

- [C++ compile and build pipeline](cpp-notes/complier/cpp_build_pipeline.md)
- [From source code to CPU: a compiler-centric view](cpp-notes/complier/compiler_to_cpu_mental_model.md)

### STL

- [Deep understanding of STL](cpp-notes/STL/stl_deep_understanding.md)
- [Toy STL vs standard STL: how far are we from `std::`?](cpp-notes/STL/stl_toy_vs_std.md)

### System

- [gRPC: concepts, architecture, and usage](cpp-notes/system/grpc_overview.md)

## Operating System Notes

- [C++ Program Lifecycle: From Executable to Page Fault](operating-system/cpp_program_lifecycle.md)
- [Study plan](operating-system/study-plan.md)
- [Week 1: Virtual Memory](operating-system/week1-virtual-memory.md)
- [Week 2: Syscall Trap](operating-system/week2-syscall-trap.md)
- [Week 3: Process Scheduling](operating-system/week3-process-scheduling.md)
- [Week 4: Locking & IO](operating-system/week4-lock-io.md)

### Concurrency Notes

- [Concurrency and systems programming](cpp-notes/concurrency/concurrency_and_systems.md)
- [Practical concurrency: threads and synchronization](cpp-notes/concurrency/concurrency_specific.md)
- [Four layers of concurrency knowledge](cpp-notes/concurrency/concurrency_layers_overview.md)
- [C++ memory order and reordering](cpp-notes/concurrency/memory_order.md)
- [Classic synchronization problems](cpp-notes/concurrency/concurrency_classic_problems.md)
- [Common concurrency challenges (checklist)](cpp-notes/concurrency/common_challenges.md)

### Infra / Performance Components (notes)

- [MemoryPool: small-object pool in practice](cpp-notes/infra/memory_pool.md)
- [ObjectPool: object lifetime management on a pool](cpp-notes/infra/object_pool.md)
- [ThreadPool: fixed thread pool and task submission](cpp-notes/infra/thread_pool.md)
- [SPSC Ring Buffer: single-producer single-consumer queue](cpp-notes/infra/spsc_ring_buffer.md)
- [CPU cache efficiency for HFT (latency context)](cpp-notes/infra/cpu_cache_efficiency_hft.md)
- [Cache organization: L1/L2/L3 and set-associative mapping](cpp-notes/infra/cache_organization_set_associative.md)
- [MemoryPool vs ObjectPool: design, responsibilities, and safety](cpp-notes/infra/mempool_vs_objectpool_safety.md)
- [Ring buffer vs mutex queue (in-depth)](cpp-notes/infra/rb_vs_mutex_indepth.md)

### Data Structures (toy implementations, notes)

- [Memory model & value semantics background](cpp-notes/fundamental_and_memory_model.md)
- [Vector / HashTable / LRUCache design notes](cpp-notes/STL/stl_deep_understanding.md)

### OOP / Polymorphism

- [OOP overall concepts](cpp-notes/oop/overall.md)
- [Static vs dynamic polymorphism](cpp-notes/oop/static_vs_dynamic_polymorphism.md)

## Codes

```text
codes/
├── includes/
│   ├── memory_pool.h
│   ├── memory_pool_safe.h
│   ├── object_pool.h
│   ├── object_pool_safe.h
│   ├── thread_pool.h
│   ├── spsc_ring_buffer.h
│   ├── vector.h
│   ├── hash_table.h
│   └── lru_cache.h
└── src/
    ├── infra/
    │   ├── memory_pool_example.cpp     # new/delete vs MemoryPool
    │   ├── object_pool_example.cpp     # std::make_unique vs ObjectPool
    │   ├── thread_pool_example.cpp     # per-task threads vs ThreadPool
    │   └── spsc_example.cpp            # mutex queue vs SpscRingBuffer
    ├── perf/
    │   └── cache_alignment_example.cpp # non-aligned vs cache-aligned counters
    ├── ds/
    │   ├── vector_example.cpp          # toy Vector<T> usage
    │   ├── hash_table_example.cpp      # toy HashTable<K, V> usage
    │   └── lru_cache_example.cpp       # toy LRUCache<K, V> usage
    ├── concurrency/
    │   └── memory_order_example.cpp    # memory_order semantics demo
    └── oop/
        ├── dynamic_virtual_example.cpp       # virtual dispatch / runtime polymorphism
        └── static_polymorphism_example.cpp   # template-based static polymorphism
```

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