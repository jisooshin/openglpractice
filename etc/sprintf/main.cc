#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <string>

using namespace std;
int main()
{
	const char* fmt = "ham%i";
	int sz = std::snprintf(nullptr, 0, fmt, 1);
	char a[sz];
	std::snprintf(&a[0], sz + 1, fmt, 1);

	string hamster(a);
	cout << hamster << endl;
	return 0;
}