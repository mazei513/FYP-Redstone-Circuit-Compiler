#include "main.hpp"

void read_chunk_file(int read_chunk[][16][16])
{
	std::ifstream in_file;
	in_file.open(chunk_name, std::ifstream::in);
	for(int y=0;y<16;y++)
	{
		for(int z=0;z<16;z++)
		{
			for(int x=0;x<16;x++)
				in_file >> read_chunk[y][z][x];
		}
	}
	
	in_file.close();
}

void print_chunk(int read_chunk[][16][16])
{
	for(int y=0;y<16;y++)
	{
		for(int z=0;z<16;z++)
		{
			for(int x=0;x<16;x++)
				std::cout << read_chunk[y][z][x] << " ";
			
			std::cout << std::endl;
		}
	}
}

std::string cur_lever_label()
{
	std::ostringstream convert;
	convert << current_lever_n;
	current_lever_n++;
	return "L" + convert.str();
}

std::string cur_torch_label()
{
	std::ostringstream convert;
	convert << current_torch_n;
	current_torch_n++;
	return "T" + convert.str();
}

std::string cur_lamp_label()
{
	std::ostringstream convert;
	convert << current_lamp_n;
	current_lamp_n++;
	return "O" + convert.str();
}

void label_components(int read_chunk[][16][16])
{
	comp_pos_label temp;
	
	for(int y=0;y<16;y++)
	{
		for(int z=0;z<16;z++)
		{
			for(int x=0;x<16;x++)
			{
				if(read_chunk[y][z][x] == REDSTONELAMP_ID || read_chunk[y][z][x] == REDSTONELAMP_ID+1)
				{
					temp = {x, y, z, cur_lamp_label(), chunk_name};
					component_labels.push_back(temp);
				}
				else if(read_chunk[y][z][x] == LEVER_ID)
				{
					temp = {x, y, z, cur_lever_label(), chunk_name};
					component_labels.push_back(temp);
				}
			}
		}
	}
}

void print_component_labels()
{
	for(int i=0;i<component_labels.size();i++)
	{
		std::cout << component_labels[i].x << " " << component_labels[i].y << " " << component_labels[i].z << " " << component_labels[i].label << std::endl;
	}
}

std::string component_name(int x, int y, int z)
{
	for(int i=0;i<component_labels.size();i++)
	{
		if(	component_labels[i].x == x &&
			component_labels[i].y == y &&
			component_labels[i].z == z &&
			component_labels[i].chunk == chunk_name)
		{
			return component_labels[i].label;
		}
	}
}

std::string find_active_component(int read_chunk[][16][16], int &x, int &y, int &z)
{
	std::string current_component;
	std::ostringstream convert;
	bool found = false;
	for(int i=0;i<16 && !found;i++)
	{
		for(int j=0;j<16&& !found;j++)
		{
			for(int k=0;k<16 && !found;k++)
			{
				if(read_chunk[i][j][k] == REDSTONELAMP_ID || read_chunk[i][j][k] == REDSTONELAMP_ID+1)
				{
					found = true;
					current_component = component_name(k, i, j);
					x = k;
					y = i;
					z = j;
				}
			}
		}
	}
	return current_component;
}

bool air_block(int x, int y, int z, int read_chunk[][16][16])
{
	if(read_chunk[y][z][x]==AIR_BLOCK_ID)
		return true;
	
	return false;
}

bool transparent_block(int x, int y, int z, int read_chunk[][16][16])
{
	for(int& i : TRANSPARENT_BLOCK_ID)
	{
		if(read_chunk[y][z][x]==TRANSPARENT_BLOCK_ID[i])
			return true;
	}
	
	return false;
}

bool component(int x, int y, int z, int read_chunk[][16][16])
{
	if(read_chunk[y][z][x]==REDSTONEDUST_ID || read_chunk[y][z][x]==REDSTONELAMP_ID || read_chunk[y][z][x]==LEVER_ID)
		return true;
	
	return false;
}

bool opaque_block(int x, int y, int z, int read_chunk[][16][16])
{
	if(!air_block(x, y, z, read_chunk) && !transparent_block(x, y, z, read_chunk) && !component(x, y, z, read_chunk))
	{
		return true;
	}
	
	return false;
}

void component_switch_case(std::vector<relationship_table> &relationships, std::string cur_component, int read_chunk[][16][16], int x, int y, int z, bool checked[][16][16])
{
	switch(read_chunk[y][z][x])
		{
			case REDSTONEDUST_ID:
				find_component_inputs(relationships, cur_component, read_chunk, x, y, z, checked);
				break;
			case LEVER_ID:
				relationship_table temp = {component_name(x, y, z), cur_component};
				relationships.push_back(temp);
				break;
		}
}

void find_component_inputs(std::vector<relationship_table> &relationships, std::string cur_component, int read_chunk[][16][16], int x, int y, int z, bool checked[][16][16])
{
	checked[y][z][x] = true;
	
	if(component(x, y, z+1, read_chunk))
	{
		if(!checked[y][z+1][x])
		{
			component_switch_case(relationships, cur_component, read_chunk, x, y, z+1, checked);
		}
	}
	else
	{
		if(!air_block(x, y, z+1, read_chunk))
		{
			if(air_block(x, y+1, z, read_chunk))
			{
				component_switch_case(relationships, cur_component, read_chunk, x, y+1, z+1, checked);
			}
		}
		else
		{
			if(opaque_block(x, y-1, z, read_chunk))
			{
				component_switch_case(relationships, cur_component, read_chunk, x, y-1, z+1, checked);
			}
		}
	}
	
	if(component(x, y, z-1, read_chunk))
	{
		if(!checked[y][z-1][x])
		{
			component_switch_case(relationships, cur_component, read_chunk, x, y, z-1, checked);
			
		}		
	}
	else
	{
		if(!air_block(x, y, z-1, read_chunk))
		{
			if(air_block(x, y+1, z, read_chunk))
			{
				component_switch_case(relationships, cur_component, read_chunk, x, y+1, z-1, checked);
			}
		}
		else
		{
			if(opaque_block(x, y-1, z, read_chunk))
			{
				component_switch_case(relationships, cur_component, read_chunk, x, y-1, z-1, checked);
			}
		}
	}
	
	if(component(x+1, y, z, read_chunk))
	{
		if(!checked[y][z][x+1])
		{
			component_switch_case(relationships, cur_component, read_chunk, x+1, y, z, checked);
		}
	}
	else
	{
		if(!air_block(x+1, y, z, read_chunk))
		{
			if(air_block(x, y+1, z, read_chunk))
			{
				component_switch_case(relationships, cur_component, read_chunk, x+1, y+1, z, checked);
			}
		}
		else
		{
			if(opaque_block(x, y-1, z, read_chunk))
			{
				component_switch_case(relationships, cur_component, read_chunk, x+1, y-1, z, checked);
			}
		}
	}
	
	if(component(x-1, y, z, read_chunk))
	{
		if(!checked[y][z][x-1])
		{
			component_switch_case(relationships, cur_component, read_chunk, x-1, y, z, checked);
		}
	}
	else
	{
		if(!air_block(x-1, y, z, read_chunk))
		{
			if(air_block(x, y+1, z, read_chunk))
			{
				component_switch_case(relationships, cur_component, read_chunk, x-1, y+1, z, checked);
			}
		}
		else
		{
			if(opaque_block(x, y-1, z, read_chunk))
			{
				component_switch_case(relationships, cur_component, read_chunk, x-1, y-1, z, checked);
			}
		}
	}
}

void find_component_inputs(std::vector<relationship_table> &relationships, std::string cur_component, int read_chunk[][16][16], int x, int y, int z)
{
	bool checked[16][16][16] = { };
	
	checked[y][z][x] = true;
	
	find_component_inputs(relationships, cur_component, read_chunk, x, y, z, checked);
}

void interpret_circuit(int read_chunk[][16][16])
{
	std::string cur_component;
	int x,y,z;
	std::vector<relationship_table> relationships;
	
	cur_component = find_active_component(read_chunk,x,y,z);
	
	find_component_inputs(relationships, cur_component, read_chunk, x, y, z);
	
	for(int i=0; i<relationships.size(); i++)
		std::cout << relationships[i].input << " => " << relationships[i].output << std::endl;
}

int main()
{
	int read_chunk[16][16][16];
	read_chunk_file(read_chunk);
	print_chunk(read_chunk);
	label_components(read_chunk);
	print_component_labels();
	
	interpret_circuit(read_chunk);
	
	return 0;
}