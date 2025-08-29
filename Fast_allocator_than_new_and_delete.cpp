// Custom allocator which is faster then new and delete
#include <iostream>
#include <chrono>
#include <vector>

template<typename T>
class MemoryPool
{
    struct FreeBlock
    {
        FreeBlock* next;
    };

    char* m_Pool;
    FreeBlock* m_FreeList;
    size_t m_BlockCount;

public:
    MemoryPool(size_t block_count)
        : m_Pool(new char[sizeof(T) * block_count]),
          m_FreeList(nullptr),
          m_BlockCount(block_count)
    {
        /* 
			Build the free list, aka the linked list of free blocks
           	The reason why we need to build this linked list is because
           	we don't know which blocks are free and which are not.
           	By building the linked list, we can easily traverse it
           	and find the next free block when we want to allocate memory
		*/

        m_FreeList = reinterpret_cast<FreeBlock*>(m_Pool);
        FreeBlock* block = m_FreeList;
        for (size_t i = 1; i < m_BlockCount; ++i)
        {
            block->next = reinterpret_cast<FreeBlock*>(m_Pool + i * sizeof(T));
            block = block->next;
        }
        block->next = nullptr;
    }

    ~MemoryPool()
    {
        delete[] m_Pool;
    }

    // Allocate a block of type T
    T* Allocate()
    {
        if (!m_FreeList)
		{
			throw std::bad_alloc();	
		}

        FreeBlock* block = m_FreeList;
        m_FreeList = m_FreeList->next;
        return reinterpret_cast<T*>(block);
    }

    // Deallocate a block of type T
    void Deallocate(T* p)
    {
        FreeBlock* block = reinterpret_cast<FreeBlock*>(p);
        block->next = m_FreeList;
        m_FreeList = block;
    }
};

// Benchmark Utility
class Benchmark
{
public:
    using Clock = std::chrono::steady_clock;

	// Takes the function as args
    template <typename Func>
    static long long measure(Func&& func)
    {
        auto start = Clock::now();
        func();
        auto end = Clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
};

// Allocation testing
class AllocatorTester
{
    size_t m_BlockCount;

public:
    AllocatorTester(size_t block_count) : m_BlockCount(block_count) {}

	// Note: I used lambda function for simplicity
	template<typename T>	
    long long TestStdNewDelete(size_t block_size)
    {
        return Benchmark::measure([&]()
        {
            std::vector<T*> ptrs;
            ptrs.reserve(m_BlockCount);

            for (size_t i = 0; i < m_BlockCount; i++)
			{
				ptrs.push_back(new T);
			}

            for (auto p : ptrs)
			{
				delete p;
			}
        });
    }

    template<typename T>
    long long TestMemoryPool(MemoryPool<T>& pool)
    {
        return Benchmark::measure([&]()
        {
            std::vector<T*> ptrs;
            ptrs.reserve(m_BlockCount);

            for (size_t i = 0; i < m_BlockCount; i++)
			{
				ptrs.push_back(pool.Allocate());
			}

            for (auto p : ptrs)
			{
				pool.Deallocate(p);
			}
        });
    }
};

int main()
{
    constexpr size_t block_count = 1'000'000;
    constexpr size_t block_size = 64;
	using Block = std::array<char, block_size>;
    MemoryPool<Block> pool(block_count);
	
    AllocatorTester tester(block_count);
    auto std_time = tester.TestStdNewDelete<Block>(block_size);
    auto pool_time = tester.TestMemoryPool(pool);

    std::cout << "Time (custom allocator): " << pool_time << " ms\n";
    std::cout << "Time (new/delete):       " << std_time << " ms\n";

    return 0;
}
