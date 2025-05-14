// Assembly program in c++
#include <iostream>
using namespace std;

int sum(int a, int b)
{
    int result;
    __asm__(
        "movl %1, %%eax;" // Move a into eax
        "addl %2, %%eax;" // Add b to eax
        "movl %%eax, %0;" // Move result from eax to result
        : "=r"(result)    // Output
        : "r"(a), "r"(b)  // Get user input
        : "%eax"          // Clobbered register
    );

    return result;
}

int main()
{
    cout << sum(4, 6);
    return 0;
}
