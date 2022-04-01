#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

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

	vector<int> a = {1, 2, 3, 4};


	cout << (&a[0] == a.data()) << endl;

	return 0;
}