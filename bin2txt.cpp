#include <iostream>
#include <fstream>
#include <string>

int main()
{
	std::ifstream fin("r.4.1.mca");
	std::ofstream fout("new.txt");
	std::string line;
	while(getline(fin, line, '\n'))
	{
		fout << line << '\n';
	}
	return 0;
}