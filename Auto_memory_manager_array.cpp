// Auto array memory manager
// ( note: Memory freed automatically when 'numbers' goes out of scope )
#include <iostream>
using namespace std;

template <typename T>
class AutoArray
{
private:
    T *ptr = nullptr;
    size_t size = 0;

public:
    // Allocates memory
    AutoArray(size_t array_size) : size(array_size)
    {
        ptr = new T[size];
        cout << "Allocated " << size << " elements\n";
    }

    // Frees memory
    ~AutoArray()
    {
        delete[] ptr;
        cout << "Freed memory\n";
    }

    // Prevent Copying to avoid issues with double deletion of the same memory
    AutoArray(const AutoArray &) = delete;
    AutoArray &operator=(const AutoArray &) = delete;

    // Simple array access
    T &operator[](size_t index)
    {
        return ptr[index];
    }

    void traverse() const
    {
        cout << "Elements of the array: [";
        for (size_t i = 0; i < size; ++i)
        {
            cout << ptr[i];
            if (i < size - 1)
                cout << ", ";
        }
        cout << "]" << endl;
    }

    size_t getSize() const { return size; }
};

int main()
{
    AutoArray<int> numbers(3);

    numbers[0] = 10;
    numbers[1] = 20;
    numbers[2] = 30;

    
    numbers.traverse();
    numbers.getSize();
    
    // Testing the copying and assignmenting
    // AutoArray<int> arr1(1);
    // AutoArray<int> arr2(1);
    // AutoArray<int> numbers = arr1;  // ERROR: Copying not allowed!
    // arr2 = arr1;                 // ERROR: Assignment not allowed!

    return 0;
}
