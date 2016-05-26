#include "nbtOps.hpp"

void chunk_list::load_chunk_list()
{
	std::ifstream in_file;
	in_file.open("chunk.lst", std::ifstream::in);
	int i=0;
	std::string temp;
	while(std::getline(in_file, temp))
	{
		temp.erase(temp.end()-1);
		chunks.push_back(temp);
	}
	in_file.close();
}

bool chunk_class::open_chunk_file(std::string path, std::ifstream& in_file)
{
	in_file.open(path.c_str());
	if(!in_file.is_open())
	{
		std::cout << "Failed to open " << path << std::endl;
		return false;
	}
	else
		return true;
}

void chunk_class::extract_section_data(std::string chunk_name)
{
	std::ifstream in_file;
	
	chunk = chunk_name;
	chunk_path = NBTFILEPATH+chunk;
	
	if(open_chunk_file(chunk_path, in_file))
	{
		std::stringstream ss;
		std::string temp;
		char c;
		bool initial = true;
		int tint;
		bool data_flipflop = true;
		
		in_file.ignore(256, 'z');
		in_file.ignore(256, ':');
		in_file >> z_pos;
		
		while(std::getline(in_file, temp))
		{
			if(temp.find("Sections")!=std::string::npos)
			{
				for(int y=0;y<16;y++)
				{
					for(int z=0;z<16;z++)
					{
						for(int x=0;x<16;x++)
						{
							if(initial)
							{
								in_file.ignore(256, '\"');
								initial=false;
							}
							in_file >> temp;
							if(temp.compare("\\")==0)
								in_file >> temp;
							tint = std::stoul(temp, nullptr, 16);
							blocks[y][z][x] = tint;
						}
					}
				}
				break;
			}
		}
		
		in_file.ignore(8000, 'Y');
		in_file.ignore(10, '\"');
		in_file >> temp;
		tint = std::stoul(temp);
		y_pos = tint;
		
		initial=true;
		in_file.ignore(8000, 'D');
		for(int y=0;y<16;y++)
		{
			for(int z=0;z<16;z++)
			{
				for(int x=0;x<16;x++)
				{
					if(initial)
					{
						in_file.ignore(256, '\"');
						initial=false;
					}
					while(in_file.get(c) && !std::isalnum(c))
					{
						
					}
					ss << c;
					ss >> temp;
					tint = std::stoul(temp, nullptr, 16);
					ss.str(std::string());
					ss.clear();
					
					if(data_flipflop)
					{
						data[y][z][x+1] = tint;
						data_flipflop = !data_flipflop;
					}
					else
					{
						data[y][z][x-1] = tint;
						data_flipflop = !data_flipflop;
					}
				}
			}
		}
		
		in_file.ignore(2000, 'x');
		in_file.ignore(256, ':');
		in_file >> x_pos;
		
		in_file.close();
	}
}

// void chunk_class::extract_block_data(std::string current_file)
// {
	// std::ifstream in_file(current_file.c_str());
	
	// if(!in_file.is_open())
	// {
		// std::cout << "Failed to open " << current_file << std::endl;
	// }
	// else
	// {
		// std::string temp;
		// char c;
		// bool initial = true;
		
		// while(std::getline(in_file, temp))
		// {
			// if(temp.find("Sections")!=std::string::npos)
			// {
				// for(int y=0;y<16;y++)
				// {
					// for(int z=0;z<16;z++)
					// {
						// for(int x=0;x<16;x++)
						// {
							// if(initial)
							// {
								// in_file.ignore(256, '\"');
								// initial=false;
							// }
							// in_file >> temp;
							// if(temp.compare("\\")==0)
								// in_file >> temp;
							// int tint = std::stoul(temp, nullptr, 16);
							// blocks[y][z][x] = tint;
						// }
					// }
				// }
			// }
		// }
		// in_file.close();
	// }
// }

void chunk_class::print_chunk()
{
	std::cout << "Extracted section : " << std::endl << "xPos : " << x_pos << std::endl << "zPos : " << z_pos << std::endl;
	
	std::cout << "Blocks :" << std::endl;
	for(int y=0;y<16;y++)
	{
		for(int z=0;z<16;z++)
		{
			for(int x=0;x<16;x++)
				std::cout << blocks[y][z][x] << " ";
			
			std::cout << std::endl;
		}
	}
	
	std::cout << "Data :" << std::endl;
	for(int y=0;y<16;y++)
	{
		for(int z=0;z<16;z++)
		{
			for(int x=0;x<16;x++)
				std::cout << data[y][z][x] << " ";
			
			std::cout << std::endl;
		}
	}
}

// int main()
// {
	// chunk_list chunks;
	// chunk_class cur_chunk;
	
	// chunks.load_chunk_list();
	
	// for(int i=0; i < chunks.size(); i++)
	// {
		// std::cout << chunks[i] << std::endl;
	// }
	
	// extract_block_data(blocks, current_file);
	// cur_chunk.extract_section_data(chunks.return_chunk(0));
	
	// cur_chunk.print_chunk();
	
	// return 0;
// }