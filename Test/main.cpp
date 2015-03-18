#include <iostream>
#include <fstream>

int main()
{
	std::ofstream testfile("E:/Projects/test.txt");

	if (testfile.good())
	{


	}
	else
	{
		std::cout << "File is bad";
		return -1;
	}


	return 0;
}