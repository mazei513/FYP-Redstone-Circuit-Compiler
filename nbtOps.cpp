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
		in_file.ignore(256, '-');
		in_file.ignore(8000, '-');
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
		
		in_file.ignore(1000000, 'x');
		in_file.ignore(256, ':');
		in_file >> x_pos;
		
		in_file.close();
	}
}

std::string chunk_class::new_chunk_str(int x, int z)
{
	std::string new_chunk = "r.";
	std::string temp_str;
	int temp_int;
	
	temp_int = x>>5;
	new_chunk += std::to_string(temp_int);
	
	new_chunk += ".";
	
	temp_int = z>>5;
	new_chunk += std::to_string(temp_int);
	
	new_chunk += ".";
	
	temp_int = (x&31) + ((z&31)<<5);
	new_chunk += std::to_string(temp_int);
	
	new_chunk += ".nbt";
	
	// std::cout << new_chunk << std::endl;
	
	return new_chunk;
}

void chunk_class::chunk_bound(int& x, int& z, int next_x, int next_z)
{
	bool load = false;
	
	// check if next coords is within chunk
	if(next_x > 15)
	{
		// shift by correct amount
		x-=16;
		
		// shift current chunk position
		x_pos++;
		load=true;
	}
	else if(next_x < 0)
	{
		x+= 16;
		x_pos--;
		load=true;
	}
	
	if(next_z > 15)
	{
		z-=16;
		z_pos++;
		load=true;
	}
	else if(next_z < 0)
	{
		z+=16;
		z_pos--;
		load=true;
	}
	
	
	if(load)
	{
		// std::cout << x_pos << " " << z_pos << std::endl;
		extract_section_data(new_chunk_str(x_pos, z_pos));
	}
}

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
		std::cout << std::endl;
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
		std::cout << std::endl;
	}
}