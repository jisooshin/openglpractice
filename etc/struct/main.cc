#include <stdio.h>
#include <iostream>

struct Hamster
{
	int    a;
	int    b;
	double c;
	float  d;
};

using namespace std;
int main()
{
	cout << offsetof(Hamster, a) << endl;
	cout << offsetof(Hamster, b) << endl;
	cout << offsetof(Hamster, c) << endl;
	cout << offsetof(Hamster, d) << endl;

	return 0;
}