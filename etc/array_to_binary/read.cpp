#include <fstream>
#include <iostream>


using namespace std;


int main()
{

	float tmp[288];
	ifstream in("vertices.dat", ios::in | ios::binary);
	in.read((char*)&tmp, sizeof(tmp));

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			cout << tmp[i + j] << " ";
		} cout << endl;
	}
	return 0;
}