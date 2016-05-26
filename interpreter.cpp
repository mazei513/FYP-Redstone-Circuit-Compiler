#include "interpreter.hpp"
#ifndef __interpreter_cpp__
#define __interpreter_cpp__

int TRANSPARENT_BLOCK_ID [] = 
{
	152, // redstone block
	62, // active furnace
	20, // glass block
	95, // stained glass
	79, // ice block
	89, // glowstone block
	91, // Jack o'lantern
	18, 161, // leaves
	73, 74, // redsotne ore
	46, // TNT
};

int current_lever_n = 1;
int current_torch_n = 1;
int current_lamp_n = 1;
std::vector<comp_pos_label> component_labels;

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

void label_components(chunk_class chunk)
{
	comp_pos_label temp;
	
	for(int y=0;y<16;y++)
	{
		for(int z=0;z<16;z++)
		{
			for(int x=0;x<16;x++)
			{
				if(chunk.return_block(x, y, z) == REDSTONELAMP_ID || chunk.return_block(x, y, z) == REDSTONELAMP_ID+1)
				{
					temp = {x, y, z, cur_lamp_label(), chunk.return_chunk()};
					component_labels.push_back(temp);
				}
				else if(chunk.return_block(x, y, z) == LEVER_ID)
				{
					temp = {x, y, z, cur_lever_label(), chunk.return_chunk()};
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

std::string component_name(int x, int y, int z, chunk_class chunk)
{
	for(int i=0;i<component_labels.size();i++)
	{
		if(	component_labels[i].x == x &&
			component_labels[i].y == y &&
			component_labels[i].z == z &&
			component_labels[i].chunk == chunk.return_chunk())
		{
			return component_labels[i].label;
		}
	}
}

std::string find_active_component(chunk_class chunk, int &x, int &y, int &z)
{
	std::string current_component;
	std::ostringstream convert;
	bool found = false;
	for(int i=0;i<16 && !found;i++)
	{
		for(int j=0;j<16 && !found;j++)
		{
			for(int k=0;k<16 && !found;k++)
			{
				if(chunk.return_block(k, i, j) == REDSTONELAMP_ID || chunk.return_block(k, i, j) == REDSTONELAMP_ID+1)
				{
					found = true;
					current_component = component_name(k, i, j, chunk);
					x = k;
					y = i;
					z = j;
					std::cerr << "Found output : " << x << " " << y << " " << z << " " << current_component << std::endl;
				}
			}
		}
	}
	return current_component;
}

bool air_block(int x, int y, int z, chunk_class chunk)
{
	if(chunk.return_block(x, y, z)==AIR_BLOCK_ID)
		return true;
	
	return false;
}

bool transparent_block(int x, int y, int z, chunk_class chunk)
{
	for(int& i : TRANSPARENT_BLOCK_ID)
	{
		if(chunk.return_block(x, y, z)==TRANSPARENT_BLOCK_ID[i])
			return true;
	}
	
	return false;
}

bool component(int x, int y, int z, chunk_class chunk)
{
	if(chunk.return_block(x, y, z)==REDSTONEDUST_ID || chunk.return_block(x, y, z)==REDSTONELAMP_ID || chunk.return_block(x, y, z)==LEVER_ID)
		return true;
	
	return false;
}

bool opaque_block(int x, int y, int z, chunk_class chunk)
{
	if(!air_block(x, y, z, chunk) && !transparent_block(x, y, z, chunk) && !component(x, y, z, chunk))
	{
		return true;
	}
	
	return false;
}

void south_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, bool checked[][16][16])
{
	relationship_table temp;
	
	if(component(x, y, z+1, chunk) && !checked[y][z+1][x])
	{
		switch(chunk.return_block(x, y, z+1))
		{
			case REDSTONEDUST_ID:
				find_component_inputs(relationships, cur_component, chunk, x, y, z+1, checked);
				break;
			case LEVER_ID:
				temp = {component_name(x, y, z+1, chunk), cur_component};
				relationships.push_back(temp);
				break;
		}
	}
	else if(opaque_block(x, y, z+1, chunk))
	{
		if(component(x, y+1, z+1, chunk) && air_block(x, y+1, z, chunk) && !checked[y+1][z+1][x])
		{
			switch(chunk.return_block(x, y+1, z+1))
			{
				case REDSTONEDUST_ID:
					find_component_inputs(relationships, cur_component, chunk, x, y+1, z+1, checked);
					break;
				case LEVER_ID:
					if(chunk.return_data(x, y+1, z+1) == 5 || chunk.return_data(x, y+1, z+1) == 6)
					{
						temp = {component_name(x, y+1, z+1, chunk), cur_component};
						relationships.push_back(temp);
					}
					break;
			}
		}
		else
		{
			if(chunk.return_block(x, y, z+2) == LEVER_ID && chunk.return_data(x, y, z+2) == 3)
			{
				temp = {component_name(x, y, z+2, chunk), cur_component};
				relationships.push_back(temp);
			}
			else if(chunk.return_block(x+1, y, z+1) == LEVER_ID && chunk.return_data(x+1, y, z+1) == 1)
			{
				temp = {component_name(x+1, y, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
			else if(chunk.return_block(x-1, y, z+1) == LEVER_ID && chunk.return_data(x-1, y, z+1) == 2)
			{
				temp = {component_name(x-1, y, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
		}
	}
	else if(component(x, y-1, z+1, chunk) && !checked[y-1][z+1][x])
	{
		switch(chunk.return_block(x, y-1, z+1))
		{
			case REDSTONEDUST_ID:
				find_component_inputs(relationships, cur_component, chunk, x, y-1, z+1, checked);
				break;
			case LEVER_ID:
				if(chunk.return_data(x, y-1, z+1) == 3)
				{
					temp = {component_name(x, y-1, z+1, chunk), cur_component};
					relationships.push_back(temp);
				}
				break;
		}
	}
}

north_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, bool checked[][16][16])
{
	relationship_table temp;
	
	if(component(x, y, z-1, chunk) && !checked[y][z-1][x])
	{
		switch(chunk.return_block(x, y, z-1))
		{
			case REDSTONEDUST_ID:
				find_component_inputs(relationships, cur_component, chunk, x, y, z-1, checked);
				break;
			case LEVER_ID:
				temp = {component_name(x, y, z-1, chunk), cur_component};
				relationships.push_back(temp);
				break;
		}
	}
	else if(opaque_block(x, y, z-1, chunk))
	{
		if(component(x, y+1, z-1, chunk) && air_block(x, y+1, z, chunk) && !checked[y+1][z-1][x])
		{
			switch(chunk.return_block(x, y+1, z-1))
			{
				case REDSTONEDUST_ID:
					find_component_inputs(relationships, cur_component, chunk, x, y+1, z-1, checked);
					break;
				case LEVER_ID:
					if(chunk.return_data(x, y+1, z-1) == 5 || chunk.return_data(x, y+1, z-1) == 6)
					{
						temp = {component_name(x, y+1, z-1, chunk), cur_component};
						relationships.push_back(temp);
					}
					break;
			}
		}
		else
		{
			if(chunk.return_block(x, y, z-2) == LEVER_ID && chunk.return_data(x, y, z-2) == 4)
			{
				temp = {component_name(x, y, z-2, chunk), cur_component};
				relationships.push_back(temp);
			}
			else if(chunk.return_block(x+1, y, z-1) == LEVER_ID && chunk.return_data(x+1, y, z-1) == 1)
			{
				temp = {component_name(x+1, y, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
			else if(chunk.return_block(x-1, y, z-1) == LEVER_ID && chunk.return_data(x-1, y, z-1) == 2)
			{
				temp = {component_name(x-1, y, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
		}
	}
	else
	{
		if(component(x, y-1, z-1, chunk) && !checked[y-1][z-1][x])
		{
			switch(chunk.return_block(x, y-1, z-1))
			{
				case REDSTONEDUST_ID:
					find_component_inputs(relationships, cur_component, chunk, x, y-1, z-1, checked);
					break;
				case LEVER_ID:
					if(chunk.return_data(x, y-1, z-1) == 4)
					{
						temp = {component_name(x, y-1, z-1, chunk), cur_component};
						relationships.push_back(temp);
					}
					break;
			}
		}
	}
}

east_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, bool checked[][16][16])
{
	relationship_table temp;
	
	if(component(x+1, y, z, chunk) && !checked[y][z][x+1])
	{
		switch(chunk.return_block(x+1, y, z))
		{
			case REDSTONEDUST_ID:
				find_component_inputs(relationships, cur_component, chunk, x+1, y, z, checked);
				break;
			case LEVER_ID:
				relationship_table temp = {component_name(x+1, y, z, chunk), cur_component};
				relationships.push_back(temp);
				break;
		}
	}
	else if(opaque_block(x+1, y, z, chunk))
	{
		if(component(x+1, y+1, z, chunk) && air_block(x, y+1, z, chunk) && !checked[x+1][y+1][z])
		{
			switch(chunk.return_block(x+1, y+1, z))
			{
				case REDSTONEDUST_ID:
					find_component_inputs(relationships, cur_component, chunk, x+1, y+1, z, checked);
					break;
				case LEVER_ID:
					if(chunk.return_data(x+1, y+1, z) == 5 || chunk.return_data(x+1, y+1, z) == 6)
					{
						relationship_table temp = {component_name(x+1, y+1, z, chunk), cur_component};
						relationships.push_back(temp);
					}
					break;
			}
		}
		else
		{
			if(chunk.return_block(x+2, y, z) == LEVER_ID && chunk.return_data(x+2, y, z) == 1)
			{
				relationship_table temp = {component_name(x+2, y, z, chunk), cur_component};
				relationships.push_back(temp);
			}
			else if(chunk.return_block(x+1, y, z-1) == LEVER_ID && chunk.return_data(x+1, y, z-1) == 4)
			{
				relationship_table temp = {component_name(x+1, y, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
			else if(chunk.return_block(x+1, y, z+1) == LEVER_ID && chunk.return_data(x+1, y, z+1) == 3)
			{
				relationship_table temp = {component_name(x+1, y, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
		}
	}
	else
	{
		if(component(x+1, y-1, z, chunk) && !checked[x+1][y-1][z])
		{
			switch(chunk.return_block(x+1, y-1, z))
			{
				case REDSTONEDUST_ID:
					find_component_inputs(relationships, cur_component, chunk, x+1, y-1, z, checked);
					break;
				case LEVER_ID:
					if(chunk.return_data(x+1, y-1, z) == 1)
					{
						relationship_table temp = {component_name(x+1, y-1, z, chunk), cur_component};
						relationships.push_back(temp);
					}
					break;
			}
		}
	}
}

west_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, bool checked[][16][16])
{
	relationship_table temp;
	
	if(component(x-1, y, z, chunk) && !checked[y][z][x-1])
	{
		switch(chunk.return_block(x-1, y, z))
		{
			case REDSTONEDUST_ID:
				find_component_inputs(relationships, cur_component, chunk, x-1, y, z, checked);
				break;
			case LEVER_ID:
				relationship_table temp = {component_name(x-1, y, z, chunk), cur_component};
				relationships.push_back(temp);
				break;
		}
	}
	else if(opaque_block(x-1, y, z, chunk))
	{
		if(component(x-1, y+1, z, chunk) && air_block(x, y+1, z, chunk) && !checked[x-1][y+1][z])
		{
			switch(chunk.return_block(x-1, y+1, z))
			{
				case REDSTONEDUST_ID:
					find_component_inputs(relationships, cur_component, chunk, x-1, y+1, z, checked);
					break;
				case LEVER_ID:
					relationship_table temp = {component_name(x-1, y+1, z, chunk), cur_component};
					relationships.push_back(temp);
					break;
			}
		}
		else
		{
			if(chunk.return_block(x-2, y, z) == LEVER_ID && chunk.return_data(x-2, y, z) == 1)
			{
				relationship_table temp = {component_name(x-2, y, z, chunk), cur_component};
				relationships.push_back(temp);
			}
			else if(chunk.return_block(x-1, y, z-1) == LEVER_ID && chunk.return_data(x-1, y, z-1) == 4)
			{
				relationship_table temp = {component_name(x-1, y, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
			else if(chunk.return_block(x-1, y, z+1) == LEVER_ID && chunk.return_data(x-1, y, z+1) == 3)
			{
				relationship_table temp = {component_name(x-1, y, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
		}
	}
	else
	{
		if(component(x-1, y-1, z, chunk) && !checked[x-1][y-1][z])
		{
			switch(chunk.return_block(x-1, y-1, z))
			{
				case REDSTONEDUST_ID:
					find_component_inputs(relationships, cur_component, chunk, x-1, y-1, z, checked);
					break;
				case LEVER_ID:
					if(chunk.return_data(x-1, y-1, z) == 2)
					{
						relationship_table temp = {component_name(x-1, y-1, z, chunk), cur_component};
						relationships.push_back(temp);
					}
					break;
			}
		}
	}
}

void find_component_inputs(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, bool checked[][16][16])
{
	checked[y][z][x] = true;
	// relationship_table temp;
	
	getchar();
	
	////// Check South //////
	south_check(relationships, cur_component, chunk, x, y, z, checked);
	
	// if(component(x, y, z+1, chunk) && !checked[y][z+1][x])
	// {
		// switch(chunk.return_block(x, y, z+1))
		// {
			// case REDSTONEDUST_ID:
				// find_component_inputs(relationships, cur_component, chunk, x, y, z+1, checked);
				// break;
			// case LEVER_ID:
				// temp = {component_name(x, y, z+1, chunk), cur_component};
				// relationships.push_back(temp);
				// break;
		// }
	// }
	// else if(opaque_block(x, y, z+1, chunk))
	// {
		// if(component(x, y+1, z+1, chunk) && air_block(x, y+1, z, chunk) && !checked[y+1][z+1][x])
		// {
			// switch(chunk.return_block(x, y+1, z+1))
			// {
				// case REDSTONEDUST_ID:
					// find_component_inputs(relationships, cur_component, chunk, x, y+1, z+1, checked);
					// break;
				// case LEVER_ID:
					// if(chunk.return_data(x, y+1, z+1) == 5 || chunk.return_data(x, y+1, z+1) == 6)
					// {
						// temp = {component_name(x, y+1, z+1, chunk), cur_component};
						// relationships.push_back(temp);
					// }
					// break;
			// }
		// }
		// else
		// {
			// if(chunk.return_block(x, y, z+2) == LEVER_ID && chunk.return_data(x, y, z+2) == 3)
			// {
				// temp = {component_name(x, y, z+2, chunk), cur_component};
				// relationships.push_back(temp);
			// }
			// else if(chunk.return_block(x+1, y, z+1) == LEVER_ID && chunk.return_data(x+1, y, z+1) == 1)
			// {
				// temp = {component_name(x+1, y, z+1, chunk), cur_component};
				// relationships.push_back(temp);
			// }
			// else if(chunk.return_block(x-1, y, z+1) == LEVER_ID && chunk.return_data(x-1, y, z+1) == 2)
			// {
				// temp = {component_name(x-1, y, z+1, chunk), cur_component};
				// relationships.push_back(temp);
			// }
		// }
	// }
	// else if(component(x, y-1, z+1, chunk) && !checked[y-1][z+1][x])
	// {
		// switch(chunk.return_block(x, y-1, z+1))
		// {
			// case REDSTONEDUST_ID:
				// find_component_inputs(relationships, cur_component, chunk, x, y-1, z+1, checked);
				// break;
			// case LEVER_ID:
				// if(chunk.return_data(x, y-1, z+1) == 3)
				// {
					// temp = {component_name(x, y-1, z+1, chunk), cur_component};
					// relationships.push_back(temp);
				// }
				// break;
		// }
	// }
	
	////// Check North //////
	north_check(relationships, cur_component, chunk, x, y, z, checked);
	
	// if(component(x, y, z-1, chunk) && !checked[y][z-1][x])
	// {
		// switch(chunk.return_block(x, y, z-1))
		// {
			// case REDSTONEDUST_ID:
				// find_component_inputs(relationships, cur_component, chunk, x, y, z-1, checked);
				// break;
			// case LEVER_ID:
				// temp = {component_name(x, y, z-1, chunk), cur_component};
				// relationships.push_back(temp);
				// break;
		// }
	// }
	// else if(opaque_block(x, y, z-1, chunk))
	// {
		// if(component(x, y+1, z-1, chunk) && air_block(x, y+1, z, chunk) && !checked[y+1][z-1][x])
		// {
			// switch(chunk.return_block(x, y+1, z-1))
			// {
				// case REDSTONEDUST_ID:
					// find_component_inputs(relationships, cur_component, chunk, x, y+1, z-1, checked);
					// break;
				// case LEVER_ID:
					// if(chunk.return_data(x, y+1, z-1) == 5 || chunk.return_data(x, y+1, z-1) == 6)
					// {
						// temp = {component_name(x, y+1, z-1, chunk), cur_component};
						// relationships.push_back(temp);
					// }
					// break;
			// }
		// }
		// else
		// {
			// if(chunk.return_block(x, y, z-2) == LEVER_ID && chunk.return_data(x, y, z-2) == 4)
			// {
				// temp = {component_name(x, y, z-2, chunk), cur_component};
				// relationships.push_back(temp);
			// }
			// else if(chunk.return_block(x+1, y, z-1) == LEVER_ID && chunk.return_data(x+1, y, z-1) == 1)
			// {
				// temp = {component_name(x+1, y, z-1, chunk), cur_component};
				// relationships.push_back(temp);
			// }
			// else if(chunk.return_block(x-1, y, z-1) == LEVER_ID && chunk.return_data(x-1, y, z-1) == 2)
			// {
				// temp = {component_name(x-1, y, z-1, chunk), cur_component};
				// relationships.push_back(temp);
			// }
		// }
	// }
	// else
	// {
		// if(component(x, y-1, z-1, chunk) && !checked[y-1][z-1][x])
		// {
			// switch(chunk.return_block(x, y-1, z-1))
			// {
				// case REDSTONEDUST_ID:
					// find_component_inputs(relationships, cur_component, chunk, x, y-1, z-1, checked);
					// break;
				// case LEVER_ID:
					// if(chunk.return_data(x, y-1, z-1) == 4)
					// {
						// temp = {component_name(x, y-1, z-1, chunk), cur_component};
						// relationships.push_back(temp);
					// }
					// break;
			// }
		// }
	// }
	
	////// Check East //////
	east_check(relationships, cur_component, chunk, x, y, z, checked);
	
	// if(component(x+1, y, z, chunk) && !checked[y][z][x+1])
	// {
		// switch(chunk.return_block(x+1, y, z))
		// {
			// case REDSTONEDUST_ID:
				// find_component_inputs(relationships, cur_component, chunk, x+1, y, z, checked);
				// break;
			// case LEVER_ID:
				// relationship_table temp = {component_name(x+1, y, z, chunk), cur_component};
				// relationships.push_back(temp);
				// break;
		// }
	// }
	// else if(opaque_block(x+1, y, z, chunk))
	// {
		// if(component(x+1, y+1, z, chunk) && air_block(x, y+1, z, chunk) && !checked[x+1][y+1][z])
		// {
			// switch(chunk.return_block(x+1, y+1, z))
			// {
				// case REDSTONEDUST_ID:
					// find_component_inputs(relationships, cur_component, chunk, x+1, y+1, z, checked);
					// break;
				// case LEVER_ID:
					// if(chunk.return_data(x+1, y+1, z) == 5 || chunk.return_data(x+1, y+1, z) == 6)
					// {
						// relationship_table temp = {component_name(x+1, y+1, z, chunk), cur_component};
						// relationships.push_back(temp);
					// }
					// break;
			// }
		// }
		// else
		// {
			// if(chunk.return_block(x+2, y, z) == LEVER_ID && chunk.return_data(x+2, y, z) == 1)
			// {
				// relationship_table temp = {component_name(x+2, y, z, chunk), cur_component};
				// relationships.push_back(temp);
			// }
			// else if(chunk.return_block(x+1, y, z-1) == LEVER_ID && chunk.return_data(x+1, y, z-1) == 4)
			// {
				// relationship_table temp = {component_name(x+1, y, z-1, chunk), cur_component};
				// relationships.push_back(temp);
			// }
			// else if(chunk.return_block(x+1, y, z+1) == LEVER_ID && chunk.return_data(x+1, y, z+1) == 3)
			// {
				// relationship_table temp = {component_name(x+1, y, z+1, chunk), cur_component};
				// relationships.push_back(temp);
			// }
		// }
	// }
	// else
	// {
		// if(component(x+1, y-1, z, chunk) && !checked[x+1][y-1][z])
		// {
			// switch(chunk.return_block(x+1, y-1, z))
			// {
				// case REDSTONEDUST_ID:
					// find_component_inputs(relationships, cur_component, chunk, x+1, y-1, z, checked);
					// break;
				// case LEVER_ID:
					// if(chunk.return_data(x+1, y-1, z) == 1)
					// {
						// relationship_table temp = {component_name(x+1, y-1, z, chunk), cur_component};
						// relationships.push_back(temp);
					// }
					// break;
			// }
		// }
	// }
	
	////// Check West //////
	west_check(relationships, cur_component, chunk, x, y, z, checked);
	
	// if(component(x-1, y, z, chunk) && !checked[y][z][x-1])
	// {
		// switch(chunk.return_block(x-1, y, z))
		// {
			// case REDSTONEDUST_ID:
				// find_component_inputs(relationships, cur_component, chunk, x-1, y, z, checked);
				// break;
			// case LEVER_ID:
				// relationship_table temp = {component_name(x-1, y, z, chunk), cur_component};
				// relationships.push_back(temp);
				// break;
		// }
	// }
	// else if(opaque_block(x-1, y, z, chunk))
	// {
		// if(component(x-1, y+1, z, chunk) && air_block(x, y+1, z, chunk) && !checked[x-1][y+1][z])
		// {
			// switch(chunk.return_block(x-1, y+1, z))
			// {
				// case REDSTONEDUST_ID:
					// find_component_inputs(relationships, cur_component, chunk, x-1, y+1, z, checked);
					// break;
				// case LEVER_ID:
					// relationship_table temp = {component_name(x-1, y+1, z, chunk), cur_component};
					// relationships.push_back(temp);
					// break;
			// }
		// }
		// else
		// {
			// if(chunk.return_block(x-2, y, z) == LEVER_ID && chunk.return_data(x-2, y, z) == 1)
			// {
				// relationship_table temp = {component_name(x-2, y, z, chunk), cur_component};
				// relationships.push_back(temp);
			// }
			// else if(chunk.return_block(x-1, y, z-1) == LEVER_ID && chunk.return_data(x-1, y, z-1) == 4)
			// {
				// relationship_table temp = {component_name(x-1, y, z-1, chunk), cur_component};
				// relationships.push_back(temp);
			// }
			// else if(chunk.return_block(x-1, y, z+1) == LEVER_ID && chunk.return_data(x-1, y, z+1) == 3)
			// {
				// relationship_table temp = {component_name(x-1, y, z+1, chunk), cur_component};
				// relationships.push_back(temp);
			// }
		// }
	// }
	// else
	// {
		// if(component(x-1, y-1, z, chunk) && !checked[x-1][y-1][z])
		// {
			// switch(chunk.return_block(x-1, y-1, z))
			// {
				// case REDSTONEDUST_ID:
					// find_component_inputs(relationships, cur_component, chunk, x-1, y-1, z, checked);
					// break;
				// case LEVER_ID:
					// if(chunk.return_data(x-1, y-1, z) == 2)
					// {
						// relationship_table temp = {component_name(x-1, y-1, z, chunk), cur_component};
						// relationships.push_back(temp);
					// }
					// break;
			// }
		// }
	// }
}

void find_component_inputs(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z)
{
	bool checked[16][16][16] = { };
	
	checked[y][z][x] = true;
	
	find_component_inputs(relationships, cur_component, chunk, x, y, z, checked);
}

void interpret_circuit(chunk_class chunk)
{
	std::string cur_component;
	int x,y,z;
	std::vector<relationship_table> relationships;
	
	cur_component = find_active_component(chunk,x,y,z);
	
	find_component_inputs(relationships, cur_component, chunk, x, y, z);
	
	for(int i=0; i<relationships.size(); i++)
		std::cout << relationships[i].input << " => " << relationships[i].output << std::endl;
}
#endif