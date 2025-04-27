# Memory Arena Explained

A **Memory Arena** (also known as a **Memory Pool**) is a custom memory management technique that allocates a large block of memory at once and then dispenses smaller chunks of that block upon request. This approach reduces overhead from frequent system allocations and deallocations, minimizes fragmentation, and improves cache performance.

**In short:**

   - A memory arena (also called a memory pool) is a big chunk of memory that you allocate once upfront, and then you carve out pieces from it manually as needed, instead of calling new or malloc every time.
---

## Table of Contents

1. [Why Use a Memory Arena?](#why-use-a-memory-arena)
2. [Core Concepts](#core-concepts)
3. [Basic Implementation](#basic-implementation)
4. [Advanced Features](#advanced-features)
5. [Use Cases](#use-cases)
6. [Best Practices](#best-practices)
7. [References](#references)

---

## Why Use a Memory Arena?

- **Performance:**
  - Single upfront allocation instead of many small allocations.
  - Fast allocation by simply bumping an offset pointer.
- **Fragmentation Control:**
  - Since the arena dispenses memory sequentially, external fragmentation is eliminated.
- **Batch Deallocation:**
  - Reset the arena in O(1) time to free all allocated objects at once.
- **Cache Friendliness:**
  - Objects allocated close in time are placed close in memory, improving cache locality.

---

## Core Concepts

1. **Arena Block**: A continuous block of memory (e.g., `char* buffer`) allocated once.
2. **Offset Pointer**: Tracks the current position in the arena where the next allocation will occur.
3. **Alignment**: Ensures that returned memory addresses conform to required alignment boundaries.
4. **Reset**: Quickly frees all allocations by resetting the offset to zero.
5. **(Optional) Free Last**: Allows freeing the most recent allocation like a stack (stack allocator).

---

## Basic Implementation

```cpp
#include <cstddef>
#include <stdexcept>

struct MemoryArena {
    char*  buffer;     // Pointer to the full memory block
    size_t capacity;   // Total size of the block
    size_t offset;     // Current allocation offset

    // Constructor: allocate the block
    MemoryArena(size_t size)
        : capacity(size), offset(0)
    {
        buffer = static_cast<char*>(::operator new(size));
    }

    // Allocate `size` bytes from the arena
    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t)) {
        // Calculate aligned offset
        size_t current = reinterpret_cast<size_t>(buffer) + offset;
        size_t aligned = (current + alignment - 1) & ~(alignment - 1);
        size_t adjustment = aligned - reinterpret_cast<size_t>(buffer) - offset;

        if (offset + adjustment + size > capacity)
            throw std::bad_alloc();

        void* ptr = buffer + offset + adjustment;
        offset += adjustment + size;
        return ptr;
    }

    // Reset the arena to reuse all memory
    void reset() {
        offset = 0;
    }

    // Destructor: free the block
    ~MemoryArena() {
        ::operator delete(buffer);
    }
};
```

**Usage Example:**

```cpp
int main() {
    MemoryArena arena(1024 * 1024); // 1 MiB arena

    // Allocate an array of 100 ints
    int* numbers = static_cast<int*>(arena.allocate(sizeof(int) * 100));

    // Use `numbers`...

    // Reset all allocations at once
    arena.reset();

    return 0;
}
```

---

## Advanced Features

### 1. Free Last Allocation (Stack Allocator)

Track individual allocations on a stack to free the most recent one:

```cpp
struct StackMarker { size_t offset; };

struct StackAllocator : MemoryArena {
    StackMarker getMarker() const { return { offset }; }
    void freeToMarker(StackMarker marker) {
        offset = marker.offset;
    }
};
```

### 2. Multiple Arenas / Sub-Arenas

Manage separate arenas for different lifetimes or object types to avoid interference.

### 3. Thread-Local Arenas

Allocate per-thread arenas to eliminate synchronization overhead in multi-threaded contexts.

---

## Use Cases

- **Game Development**: Fast, predictable allocations during frame updates.
- **Real-Time Systems**: Low-latency memory operations.
- **Embedded Systems**: Control over limited memory budgets.
- **Custom Allocators**: Backend for STL containers or domain-specific allocators.

---

## Best Practices

- **Sizing:** Choose an arena size based on peak memory usage patterns.
- **Alignment:** Always handle alignment to avoid undefined behavior.
- **Lifetime Management:** Reset only when all objects in the arena are no longer needed.
- **Error Handling:** Gracefully handle out-of-memory conditions.

---
