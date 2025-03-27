// Accurate and efficient random number generator
#include <iostream>
#include <random>
#include <time.h>
using namespace std;

// Seed generator once like srand(time(0))
mt19937 rng(time(0)); 

int RandomNumGen(int a, int z)
{
    uniform_int_distribution<int> dist(a, z); // Define the range
    return dist(rng);                         
}

int main()
{
    cout << "Random number is: " << RandomNumGen(1, 50) << endl;
    return 0;
}
