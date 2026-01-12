// Custom allocator which is faster then new and delete
#include <iostream>
#include <chrono>
#include <array>
#include <vector>

template<typename T>
class MemoryPool
{
    struct t_FreeBlock
    {
        t_FreeBlock* next;
    };

    char* m_Pool;
    t_FreeBlock* m_FreeList;
    size_t m_BlockCount;

public:
    MemoryPool(const size_t block_count)
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

        m_FreeList = reinterpret_cast<t_FreeBlock*>(m_Pool);
        t_FreeBlock* block = m_FreeList;
        for (size_t i = 1; i < m_BlockCount; ++i)
        {
            block->next = reinterpret_cast<t_FreeBlock*>(m_Pool + i * sizeof(T));
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

        t_FreeBlock* block = m_FreeList;
        m_FreeList = m_FreeList->next;
        return reinterpret_cast<T*>(block);
    }

    // Deallocate a block of type T
    void Deallocate(T* p)
    {
        t_FreeBlock* block = reinterpret_cast<t_FreeBlock*>(p);
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
    AllocatorTester(const size_t block_count) : m_BlockCount(block_count) {}

	// Note: I used lambda function for simplicity
	template<typename T>	
    long long TestStdNewDelete(const size_t block_size)
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
    constexpr size_t BLOCK_COUNT = 1'000'000;
    constexpr size_t BLOCK_SIZE = 64;
	using Block = std::array<char, BLOCK_SIZE>;
    MemoryPool<Block> pool(BLOCK_COUNT);
	
    AllocatorTester tester(BLOCK_COUNT);
    auto std_time = tester.TestStdNewDelete<Block>(BLOCK_SIZE);
    auto pool_time = tester.TestMemoryPool(pool);

    std::cout << "Time (custom allocator): " << pool_time << " ms\n";
    std::cout << "Time (new/delete):       " << std_time << " ms\n";

    return 0;
}
