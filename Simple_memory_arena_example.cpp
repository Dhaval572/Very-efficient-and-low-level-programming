// Creating a simple memory arena
#include <iostream>

struct Arena
{
	char *memory;
	size_t capacity;
	size_t offset;

	Arena(size_t size) : capacity(size), offset(0)
	{
		memory = new char[size];
	}

	void *allocate(size_t size)
	{
		if (offset + size > capacity)
			throw std::bad_alloc();

		void *ptr = memory + offset;
		offset += size;
		return ptr;
	}

	void reset()
	{
		offset = 0;
	}

	~Arena()
	{
		delete[] memory;
	}
};

int main()
{
	Arena arena(1024); // 1 KB arena

	int *arr = (int *)arena.allocate(sizeof(int) * 100); // allocate array of 100 ints
}
