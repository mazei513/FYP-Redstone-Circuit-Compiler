#include <iostream>
#include <fstream>
#include <string>

int main()
{
	std::ifstream fin("58.nbt", std::fstream::binary);
	std::ofstream fout("new.txt");
	std::string line;
	while(getline(fin, line, '\n'))
	{
		fout << line << '\n';
	}
	return 0;
}
