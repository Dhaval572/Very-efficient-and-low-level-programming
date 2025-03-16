// Checking if a number is even or odd (Fastest way)
#include <iostream>
using namespace std;

int main()
{
	int x = 5;
	if (x & 1)
		cout << "Odd";
	else
		cout << "Even";

	return 0;
}
