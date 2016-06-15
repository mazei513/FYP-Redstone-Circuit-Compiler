#ifndef __interpreter_cpp__
#define __interpreter_cpp__
#include "interpreter.hpp"

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
std::vector<comp_pos_label> active_components;

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
					active_components.push_back(temp);
				}
				else if(chunk.return_block(x, y, z) == LEVER_ID)
				{
					temp = {x, y, z, cur_lever_label(), chunk.return_chunk()};
					component_labels.push_back(temp);
				}
				else if(chunk.return_block(x, y, z) == REDSTONETORCH_ID || chunk.return_block(x, y, z) == REDSTONETORCH_ID+1)
				{
					temp = {x, y, z, cur_torch_label(), chunk.return_chunk()};
					component_labels.push_back(temp);
					active_components.push_back(temp);
				}
			}
		}
	}
}

void print_component_labels()
{
	for(int i=0;i<component_labels.size();i++)
	{
		std::cout << component_labels[i].x << " " << component_labels[i].y << " " << component_labels[i].z << " " << component_labels[i].label << " "  << component_labels[i].chunk << std::endl;
	}
}

void print_active_component_labels()
{
	for(int i=0;i<active_components.size();i++)
	{
		std::cout << active_components[i].x << " " << active_components[i].y << " " << active_components[i].z << " " << active_components[i].label << " "  << active_components[i].chunk << std::endl;
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

bool is_lamp(int x, int y, int z, chunk_class chunk)
{
	return chunk.return_block(x, y, z) == REDSTONELAMP_ID || chunk.return_block(x, y, z) == REDSTONELAMP_ID+1;
}

bool is_torch(int x, int y, int z, chunk_class chunk)
{
	return chunk.return_block(x, y, z) == REDSTONETORCH_ID || chunk.return_block(x, y, z) == REDSTONETORCH_ID+1;
}

bool is_lever(int x, int y, int z, chunk_class chunk)
{
	return chunk.return_block(x, y, z) == LEVER_ID;
}

bool is_dust(int x, int y, int z, chunk_class chunk)
{
	return chunk.return_block(x, y, z) == REDSTONEDUST_ID;
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
				if(is_lamp(k, i, j, chunk))
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
	if(is_dust(x, y, z, chunk) || is_lamp(x, y, z, chunk) || is_lever(x, y, z, chunk) || is_torch(x, y, z, chunk))
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

void south_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power)
{
	if(power == 0)
		return;
	relationship_table temp;
	
	// check dust adjacent
	chunk.chunk_bound(x, z, x, z+1);
	if(is_dust(x, y, z+1, chunk) && !checked[offset_y][offset_z+1][offset_x].checked)
			find_component_inputs(relationships, cur_component, chunk, x, y, z+1, offset_x, offset_y, offset_z+1, checked, power-1);
	// else check lever adjacent
	else if(is_lever(x, y, z+1, chunk))
	{
		temp = {component_name(x, y, z+1, chunk), cur_component};
		relationships.push_back(temp);
	}
	// else check torch adjacent
	else if(is_torch(x, y, z+1, chunk))
	{
		temp = {component_name(x, y, z+1, chunk), cur_component};
		relationships.push_back(temp);
	}
	// else check if opaque block adjacent
	else if(opaque_block(x, y, z+1, chunk))
	{
		// check dust above block
		if(is_dust(x, y+1, z+1, chunk) && !checked[offset_y+1][offset_z+1][offset_x].checked)
		{
			chunk.chunk_bound(x, z, x, z);
			if(!opaque_block(x, y+1, z, chunk) || !is_dust(x, y, z, chunk))
				find_component_inputs(relationships, cur_component, chunk, x, y+1, z+1, offset_x, offset_y+1, offset_z+1, checked, power-1);
			chunk.chunk_bound(x, z, x, z+1);
		}
		else
		{
			chunk.chunk_bound(x, z, x, z);
			if(!is_dust(x, y, z, chunk))
			{
				chunk.chunk_bound(x, z, x, z+2);
				if(is_dust(x, y, z+2, chunk))
					find_component_inputs(relationships, cur_component, chunk, x, y, z+2, offset_x, offset_y, offset_z+2, checked, power-1);
				chunk.chunk_bound(x, z, x+1, z+1);
				if(is_dust(x+1, y, z+1, chunk))
					find_component_inputs(relationships, cur_component, chunk, x+1, y, z+1, offset_x+1, offset_y, offset_z+1, checked, power-1);
				chunk.chunk_bound(x, z, x-1, z+1);
				if(is_dust(x-1, y, z+1, chunk))
					find_component_inputs(relationships, cur_component, chunk, x-1, y, z+1, offset_x-1, offset_y, offset_z+1, checked, power-1);
			}
			// else check lever on all sides of block
			chunk.chunk_bound(x, z, x, z+1);
			if(is_lever(x, y+1, z+1, chunk) && (chunk.return_data(x, y+1, z+1) == 5 || chunk.return_data(x, y+1, z+1) == 6 || chunk.return_data(x, y+1, z+1) == 13 || chunk.return_data(x, y+1, z+1) == 14))
			{
				temp = {component_name(x, y+1, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x, z+2);
			if(is_lever(x, y, z+2, chunk) && (chunk.return_data(x, y, z+2) == 3 || chunk.return_data(x, y, z+2) == 11))
			{
				temp = {component_name(x, y, z+2, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x+1, z+1);
			if(is_lever(x+1, y, z+1, chunk) && (chunk.return_data(x+1, y, z+1) == 1 || chunk.return_data(x+1, y, z+1) == 9))
			{
				temp = {component_name(x+1, y, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x-1, z+1);
			if(is_lever(x-1, y, z+1, chunk) && (chunk.return_data(x-1, y, z+1) == 2 || chunk.return_data(x-1, y, z+1) == 10))
			{
				temp = {component_name(x-1, y, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
			// for power source from underneath opaque block, check for torches as well
			chunk.chunk_bound(x, z, x, z+1);
			if(is_torch(x, y-1, z+1, chunk) || (is_lever(x, y-1, z+1, chunk) && (chunk.return_data(x, y-1, z+1) == 0 || chunk.return_data(x, y-1, z+1) == 7 || chunk.return_data(x, y-1, z+1) == 3 || chunk.return_data(x, y-1, z+1) == 8 || chunk.return_data(x, y-1, z+1) == 15 || chunk.return_data(x, y-1, z+1) == 11)))
			{
				temp = {component_name(x, y-1, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
		}
	}
	// else check dust from adjacent below
	else if(is_dust(x, y-1, z+1, chunk) && !checked[offset_y-1][offset_z+1][offset_x].checked)
	{
		if(is_dust(x+1, y-1, z+1, chunk) || (is_dust(x-1, y-1, z+1, chunk)))
		{
			if(is_dust(x, y, z, chunk))
				find_component_inputs(relationships, cur_component, chunk, x, y-1, z+1, offset_x, offset_y-1, offset_z+1, checked, power-1);
		}
		else
		{			
			if((is_dust(x+1, y-2, z+1, chunk) && !opaque_block(x-1, y-1, z+1, chunk)) && (is_dust(x+1, y-2, z+1, chunk) && !opaque_block(x-1, y-1, z+1, chunk)) && ((is_dust(x+1, y, z+1, chunk) || (is_dust(x-1, y, z+1, chunk))) && !opaque_block(x+1, y, z, chunk)))
			{
				if(is_dust(x, y, z, chunk))
					find_component_inputs(relationships, cur_component, chunk, x, y-1, z+1, offset_x, offset_y-1, offset_z+1, checked, power-1);
			}
			else
			{
				find_component_inputs(relationships, cur_component, chunk, x, y-1, z+1, offset_x, offset_y-1, offset_z+1, checked, power-1);
			}
		}
	}
	else if(is_lever(x, y-1, z+1, chunk) && (chunk.return_data(x, y-1, z+1) == 3 || chunk.return_data(x, y-1, z+1) == 11))
	{
		temp = {component_name(x, y-1, z+1, chunk), cur_component};
		relationships.push_back(temp);
	}
	chunk.chunk_bound(x, z, x, z);
}

void north_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power)
{
	if(power == 0)
		return;
	relationship_table temp;
	
	// check dust adjacent
	chunk.chunk_bound(x, z, x, z-1);
	if(is_dust(x, y, z-1, chunk) && !checked[offset_y][offset_z-1][offset_x].checked)
			find_component_inputs(relationships, cur_component, chunk, x, y, z-1, offset_x, offset_y, offset_z-1, checked, power-1);
	// else check lever adjacent
	else if(is_lever(x, y, z-1, chunk))
	{
		temp = {component_name(x, y, z-1, chunk), cur_component};
		relationships.push_back(temp);
	}
	// else check torch adjacent
	else if(is_torch(x, y, z-1, chunk))
	{
		temp = {component_name(x, y, z-1, chunk), cur_component};
		relationships.push_back(temp);
	}
	// else check if opaque block adjacent
	else if(opaque_block(x, y, z-1, chunk))
	{
		// check dust above block
		if(is_dust(x, y+1, z-1, chunk) && !checked[offset_y+1][offset_z-1][offset_x].checked)
		{
			chunk.chunk_bound(x, z, x, z);
			if(!opaque_block(x, y+1, z, chunk) || !is_dust(x, y, z, chunk))
				find_component_inputs(relationships, cur_component, chunk, x, y+1, z-1, offset_x, offset_y+1, offset_z-1, checked, power-1);
			chunk.chunk_bound(x, z, x, z-1);
		}
		else 
		{
			chunk.chunk_bound(x, z, x, z);
			if(!is_dust(x, y, z, chunk))
			{
				chunk.chunk_bound(x, z, x, z-2);
				if(is_dust(x, y, z-2, chunk))
					find_component_inputs(relationships, cur_component, chunk, x, y, z-2, offset_x, offset_y, offset_z-2, checked, power-1);
				chunk.chunk_bound(x, z, x-1, z-1);
				if(is_dust(x-1, y, z-1, chunk))
					find_component_inputs(relationships, cur_component, chunk, x-1, y, z-1, offset_x-1, offset_y, offset_z-1, checked, power-1);
				chunk.chunk_bound(x, z, x+1, z-1);
				if(is_dust(x+1, y, z-1, chunk))
					find_component_inputs(relationships, cur_component, chunk, x+1, y, z-1, offset_x+1, offset_y, offset_z-1, checked, power-1);
			}
			// else check lever on all sides of block
			chunk.chunk_bound(x, z, x, z-1);
			if(is_lever(x, y+1, z-1, chunk) && (chunk.return_data(x, y+1, z-1) == 5 || chunk.return_data(x, y+1, z-1) == 6 || chunk.return_data(x, y+1, z-1) == 13 || chunk.return_data(x, y+1, z-1) == 14))
			{
				temp = {component_name(x, y+1, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x, z-2);
			if(is_lever(x, y, z-2, chunk) && (chunk.return_data(x, y, z-2) == 4 || chunk.return_data(x, y, z-2) == 12))
			{
				temp = {component_name(x, y, z-2, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x+1, z-1);
			if(is_lever(x+1, y, z-1, chunk) && (chunk.return_data(x+1, y, z-1) == 1 || chunk.return_data(x+1, y, z-1) == 9))
			{
				temp = {component_name(x+1, y, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x-1, z-1);
			if(is_lever(x-1, y, z-1, chunk) && (chunk.return_data(x-1, y, z-1) == 2 || chunk.return_data(x-1, y, z-1) == 10))
			{
				temp = {component_name(x-1, y, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
			// for power source from underneath opaque block, check for torches as well
			chunk.chunk_bound(x, z, x, z-1);
			if(is_torch(x, y-1, z-1, chunk) || (is_lever(x, y-1, z-1, chunk) && (chunk.return_data(x, y-1, z-1) == 0 || chunk.return_data(x, y-1, z-1) == 7 || chunk.return_data(x, y-1, z-1) == 4 || chunk.return_data(x, y-1, z-1) == 8 || chunk.return_data(x, y-1, z-1) == 15 || chunk.return_data(x, y-1, z-1) == 12)))
			{
				temp = {component_name(x, y-1, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
		}
	}
	// else check dust from adjacent below
	else if(is_dust(x, y-1, z-1, chunk) && !checked[offset_y-1][offset_z-1][offset_x].checked)
	{
		if(is_dust(x+1, y-1, z-1, chunk) || (is_dust(x-1, y-1, z-1, chunk)))
		{
			if(is_dust(x, y, z, chunk))
				find_component_inputs(relationships, cur_component, chunk, x, y-1, z-1, offset_x, offset_y-1, offset_z-1, checked, power-1);
		}
		else
		{			
			if((is_dust(x+1, y-2, z-1, chunk) && !opaque_block(x-1, y-1, z-1, chunk)) && (is_dust(x+1, y-2, z-1, chunk) && !opaque_block(x-1, y-1, z-1, chunk)) && ((is_dust(x+1, y, z-1, chunk) || (is_dust(x-1, y, z-1, chunk))) && !opaque_block(x+1, y, z, chunk)))
			{
				if(is_dust(x, y, z, chunk))
					find_component_inputs(relationships, cur_component, chunk, x, y-1, z-1, offset_x, offset_y-1, offset_z-1, checked, power-1);
			}
			else
			{
				find_component_inputs(relationships, cur_component, chunk, x, y-1, z-1, offset_x, offset_y-1, offset_z-1, checked, power-1);
			}
		}
	}
	else if(is_lever(x, y-1, z-1, chunk) && (chunk.return_data(x, y-1, z-1) == 4 || chunk.return_data(x, y-1, z-1) == 12))
	{
		temp = {component_name(x, y-1, z-1, chunk), cur_component};
		relationships.push_back(temp);
	}
	chunk.chunk_bound(x, z, x, z);
}

void east_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power)
{
	if(power == 0)
		return;
	relationship_table temp;
	
	// check dust adjacent
	chunk.chunk_bound(x, z, x+1, z);
	checked[offset_y][offset_z][offset_x+1].chunk = chunk.return_chunk();
	if(is_dust(x+1, y, z, chunk) && !checked[offset_y][offset_z][offset_x+1].checked)
		find_component_inputs(relationships, cur_component, chunk, x+1, y, z, offset_x+1, offset_y, offset_z, checked, power-1);
	// else check lever adjacent
	else if(is_lever(x+1, y, z, chunk))
	{
		temp = {component_name(x+1, y, z, chunk), cur_component};
		relationships.push_back(temp);
	}
	// else check torch adjacent
	else if(is_torch(x+1, y, z, chunk))
	{
		temp = {component_name(x+1, y, z, chunk), cur_component};
		relationships.push_back(temp);
	}
	// else check if opaque block adjacent
	else if(opaque_block(x+1, y, z, chunk))
	{
		// check dust above block
		if(is_dust(x+1, y+1, z, chunk) && !checked[offset_y+1][offset_z][offset_x+1].checked)
		{
			chunk.chunk_bound(x, z, x, z);
			if(!opaque_block(x, y+1, z, chunk) || !is_dust(x, y, z, chunk))
				find_component_inputs(relationships, cur_component, chunk, x+1, y+1, z, offset_x+1, offset_y+1, offset_z, checked, power-1);
			chunk.chunk_bound(x, z, x+1, z);
		}
		else 
		{
			chunk.chunk_bound(x, z, x, z);
			if(!is_dust(x, y, z, chunk))
			{
				chunk.chunk_bound(x, z, x+2, z);
				if(is_dust(x+2, y, z, chunk))
					find_component_inputs(relationships, cur_component, chunk, x+2, y, z, offset_x+2, offset_y, offset_z, checked, power-1);
				chunk.chunk_bound(x, z, x+1, z+1);
				if(is_dust(x+1, y, z+1, chunk))
					find_component_inputs(relationships, cur_component, chunk, x+1, y, z+1, offset_x+1, offset_y, offset_z+1, checked, power-1);
				chunk.chunk_bound(x, z, x+1, z-1);
				if(is_dust(x+1, y, z-1, chunk))
					find_component_inputs(relationships, cur_component, chunk, x+1, y, z-1, offset_x+1, offset_y, offset_z-1, checked, power-1);
			}
			// else check lever on all sides of block
			chunk.chunk_bound(x, z, x+1, z);
			if(is_lever(x+1, y+1, z, chunk) && (chunk.return_data(x+1, y+1, z) == 5 || chunk.return_data(x+1, y+1, z) == 6 || chunk.return_data(x+1, y+1, z) == 13 || chunk.return_data(x+1, y+1, z) == 14))
			{
				temp = {component_name(x+1, y+1, z, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x+2, z);
			if(is_lever(x+2, y, z, chunk) && (chunk.return_data(x+2, y, z) == 1 || chunk.return_data(x+2, y, z) == 9))
			{
				temp = {component_name(x+2, y, z, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x+1, z-1);
			if(is_lever(x+1, y, z-1, chunk) && (chunk.return_data(x+1, y, z-1) == 4 || chunk.return_data(x+1, y, z-1) == 12))
			{
				temp = {component_name(x+1, y, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x+1, z+1);
			if(is_lever(x+1, y, z+1, chunk) && (chunk.return_data(x+1, y, z+1) == 3 || chunk.return_data(x+1, y, z+1) == 11))
			{
				temp = {component_name(x+1, y, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
			// for power source from underneath opaque block, check for torches as well
			
			chunk.chunk_bound(x, z, x, z);
			if(is_torch(x+1, y-1, z, chunk) || (is_lever(x+1, y-1, z, chunk) && (chunk.return_data(x+1, y-1, z) == 0 || chunk.return_data(x+1, y-1, z) == 7 || chunk.return_data(x+1, y-1, z) == 1 || chunk.return_data(x+1, y-1, z) == 8 || chunk.return_data(x+1, y-1, z) == 15 || chunk.return_data(x+1, y-1, z) == 9)))
			{
				temp = {component_name(x+1, y-1, z, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x+1, z);
		}
	}
	// else check dust from adjacent below
	else if(is_dust(x+1, y-1, z, chunk) && !checked[offset_y-1][offset_z][offset_x+1].checked)
	{
		if(is_dust(x+1, y-1, z+1, chunk) || (is_dust(x+1, y-1, z-1, chunk)))
		{
			if(is_dust(x, y, z, chunk))
				find_component_inputs(relationships, cur_component, chunk, x+1, y-1, z, offset_x+1, offset_y-1, offset_z, checked, power-1);
		}
		else
		{			
			if((is_dust(x+1, y-2, z+1, chunk) && !opaque_block(x+1, y-1, z+1, chunk)) && (is_dust(x+1, y-2, z-1, chunk) && !opaque_block(x+1, y-1, z-1, chunk)) && ((is_dust(x+1, y, z+1, chunk) || (is_dust(x+1, y, z-1, chunk))) && !opaque_block(x+1, y, z, chunk)))
			{
				if(is_dust(x, y, z, chunk))
					find_component_inputs(relationships, cur_component, chunk, x+1, y-1, z, offset_x+1, offset_y-1, offset_z, checked, power-1);
			}
			else
			{
				find_component_inputs(relationships, cur_component, chunk, x+1, y-1, z, offset_x+1, offset_y-1, offset_z, checked, power-1);
			}
		}
	}
	else if(is_lever(x+1, y-1, z, chunk) && (chunk.return_data(x+1, y-1, z) == 1 || chunk.return_data(x+1, y-1, z) == 9))
	{
		temp = {component_name(x+1, y-1, z, chunk), cur_component};
		relationships.push_back(temp);
	}
	chunk.chunk_bound(x, z, x, z);
}

void west_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power)
{
	if(power == 0)
		return;
	relationship_table temp;
	
	// check dust adjacent
	chunk.chunk_bound(x, z, x-1, z);
	if(is_dust(x-1, y, z, chunk) && !checked[offset_y][offset_z][offset_x-1].checked)
		find_component_inputs(relationships, cur_component, chunk, x-1, y, z, offset_x-1, offset_y, offset_z, checked, power-1);
	// else check lever adjacent
	else if(is_lever(x-1, y, z, chunk))
	{
		temp = {component_name(x-1, y, z, chunk), cur_component};
		relationships.push_back(temp);
	}
	// else check torch adjacent
	else if(is_torch(x-1, y, z, chunk))
	{
		temp = {component_name(x-1, y, z, chunk), cur_component};
		relationships.push_back(temp);
	}
	// else check if opaque block adjacent
	else if(opaque_block(x-1, y, z, chunk))
	{
		// check dust above block
		if(is_dust(x-1, y+1, z, chunk) && !checked[offset_y+1][offset_z][offset_x-1].checked)
		{
			chunk.chunk_bound(x, z, x, z);
			if(!opaque_block(x, y+1, z, chunk) || !is_dust(x, y, z, chunk))
				find_component_inputs(relationships, cur_component, chunk, x-1, y+1, z, offset_x-1, offset_y+1, offset_z, checked, power-1);
			chunk.chunk_bound(x, z, x-1, z);
		}
		else 
		{
			chunk.chunk_bound(x, z, x, z);
			if(!is_dust(x, y, z, chunk))
			{
				chunk.chunk_bound(x, z, x-2, z);
				if(is_dust(x-2, y, z, chunk))
					find_component_inputs(relationships, cur_component, chunk, x-2, y, z, offset_x-2, offset_y, offset_z, checked, power-1);
				chunk.chunk_bound(x, z, x-1, z+1);
				if(is_dust(x-1, y, z+1, chunk))
					find_component_inputs(relationships, cur_component, chunk, x-1, y, z+1, offset_x-1, offset_y, offset_z+1, checked, power-1);
				chunk.chunk_bound(x, z, x-1, z-1);
				if(is_dust(x-1, y, z-1, chunk))
					find_component_inputs(relationships, cur_component, chunk, x-1, y, z-1, offset_x-1, offset_y, offset_z-1, checked, power-1);
				
			}
			chunk.chunk_bound(x, z, x-1, z);
			// else check lever on all sides of block
			if(is_lever(x-1, y+1, z, chunk) && (chunk.return_data(x-1, y+1, z) == 5 || chunk.return_data(x-1, y+1, z) == 6 || chunk.return_data(x-1, y+1, z) == 13 || chunk.return_data(x-1, y+1, z) == 14))
			{
				temp = {component_name(x-1, y+1, z, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x-2, z);
			if(is_lever(x-2, y, z, chunk) && (chunk.return_data(x-2, y, z) == 2 || chunk.return_data(x-2, y, z) == 10))
			{
				temp = {component_name(x-2, y, z, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x-1, z-1);
			if(is_lever(x-1, y, z-1, chunk) && (chunk.return_data(x-1, y, z-1) == 4 || chunk.return_data(x-1, y, z-1) == 12))
			{
				temp = {component_name(x-1, y, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x-1, z+1);
			if(is_lever(x-1, y, z+1, chunk) && (chunk.return_data(x-1, y, z+1) == 3 || chunk.return_data(x-1, y, z+1) == 11))
			{
				temp = {component_name(x-1, y, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x-1, z);
			// for power source from underneath opaque block, check for torches as well
			if(is_torch(x-1, y-1, z, chunk) || (is_lever(x-1, y-1, z, chunk) && (chunk.return_data(x-1, y-1, z) == 0 || chunk.return_data(x-1, y-1, z) == 7 || chunk.return_data(x-1, y-1, z) == 2 || chunk.return_data(x-1, y-1, z) == 8 || chunk.return_data(x-1, y-1, z) == 15 || chunk.return_data(x-1, y-1, z) == 10)))
			{
				temp = {component_name(x-1, y-1, z, chunk), cur_component};
				relationships.push_back(temp);
			}
		}
	}
	// else check dust from adjacent below
	else if(is_dust(x-1, y-1, z, chunk) && !checked[offset_y-1][offset_z][offset_x-1].checked)
	{
		if(is_dust(x-1, y-1, z+1, chunk) || (is_dust(x-1, y-1, z-1, chunk)))
		{
			if(is_dust(x, y, z, chunk))
				find_component_inputs(relationships, cur_component, chunk, x-1, y-1, z, offset_x-1, offset_y-1, offset_z, checked, power-1);
		}
		else
		{			
			if((is_dust(x-1, y-2, z+1, chunk) && !opaque_block(x-1, y-1, z+1, chunk)) && (is_dust(x-1, y-2, z-1, chunk) && !opaque_block(x-1, y-1, z-1, chunk)) && ((is_dust(x-1, y, z+1, chunk) || (is_dust(x-1, y, z-1, chunk))) && !opaque_block(x-1, y, z, chunk)))
			{
				if(is_dust(x, y, z, chunk))
					find_component_inputs(relationships, cur_component, chunk, x-1, y-1, z, offset_x-1, offset_y-1, offset_z, checked, power-1);
			}
			else
			{
				find_component_inputs(relationships, cur_component, chunk, x-1, y-1, z, offset_x-1, offset_y-1, offset_z, checked, power-1);
			}
		}
	}
	else if(is_lever(x-1, y-1, z, chunk) && (chunk.return_data(x-1, y-1, z) == 2 || chunk.return_data(x-1, y-1, z) == 10))
	{
		temp = {component_name(x-1, y-1, z, chunk), cur_component};
		relationships.push_back(temp);
	}
	chunk.chunk_bound(x, z, x, z);
}

void top_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power)
{
	if(power == 0)
		return;
	relationship_table temp;
	
	if(opaque_block(x, y+1, z, chunk))
	{
		chunk.chunk_bound(x, z, x, z);
		if(is_lever(x, y+2, z, chunk) && (chunk.return_data(x, y+2, z) == 5 || chunk.return_data(x, y+2, z) == 6 || chunk.return_data(x, y+2, z) == 13 || chunk.return_data(x, y+2, z) == 14))
		{
			temp = {component_name(x, y+2, z, chunk), cur_component};
			relationships.push_back(temp);
		}
		chunk.chunk_bound(x, z, x-1, z);
		if(is_lever(x-1, y+1, z, chunk) && (chunk.return_data(x-1, y+1, z) == 2 || chunk.return_data(x-1, y+1, z) == 10))
		{
			temp = {component_name(x-1, y+1, z, chunk), cur_component};
			relationships.push_back(temp);
		}
		chunk.chunk_bound(x, z, x+1, z);
		if(is_lever(x+1, y+1, z, chunk) && (chunk.return_data(x+1, y+1, z) == 1 || chunk.return_data(x+1, y+1, z) == 9))
		{
			temp = {component_name(x+1, y+1, z, chunk), cur_component};
			relationships.push_back(temp);
		}
		chunk.chunk_bound(x, z, x, z+1);
		if(is_lever(x, y+1, z+1, chunk) && (chunk.return_data(x, y+1, z+1) == 3 || chunk.return_data(x, y+1, z+1) == 11))
		{
			temp = {component_name(x, y+1, z+1, chunk), cur_component};
			relationships.push_back(temp);
		}
		chunk.chunk_bound(x, z, x, z-1);
		if(is_lever(x, y+1, z-1, chunk) && (chunk.return_data(x, y+1, z-1) == 4 || chunk.return_data(x, y+1, z-1) == 12))
		{
			temp = {component_name(x, y+1, z-1, chunk), cur_component};
			relationships.push_back(temp);
		}
	}
	chunk.chunk_bound(x, z, x, z);
}

void bottom_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power)
{
	if(power == 0)
		return;
	relationship_table temp;
	
	if(is_torch(x, y-1, z, chunk) || (is_lever(x, y-1, z, chunk) && (chunk.return_data(x, y-1, z) == 0 || chunk.return_data(x, y-1, z) == 7 || chunk.return_data(x, y-1, z) == 8 || chunk.return_data(x, y-1, z) == 15)))
	{
		temp = {component_name(x, y-1, z, chunk), cur_component};
		relationships.push_back(temp);
	}
	else
	{
		chunk.chunk_bound(x, z, x, z);
		if(opaque_block(x, y-1, z, chunk))
		{
			if(is_torch(x, y-2, z, chunk) || (is_lever(x, y-2, z, chunk) && (chunk.return_data(x, y-2, z) == 0 || chunk.return_data(x, y-2, z) == 7 || chunk.return_data(x, y-2, z) == 8 || chunk.return_data(x, y-2, z) == 15)))
			{
				temp = {component_name(x, y-2, z, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x-1, z);
			if(is_lever(x-1, y-1, z, chunk) && (chunk.return_data(x-1, y-1, z) == 2 || chunk.return_data(x-1, y-1, z) == 10))
			{
				temp = {component_name(x-1, y-1, z, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x+1, z);
			if(is_lever(x+1, y-1, z, chunk) && (chunk.return_data(x+1, y-1, z) == 1 || chunk.return_data(x+1, y-1, z) == 9))
			{
				temp = {component_name(x+1, y-1, z, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x, z+1);
			if(is_lever(x, y-1, z+1, chunk) && (chunk.return_data(x, y-1, z+1) == 3 || chunk.return_data(x, y-1, z+1) == 11))
			{
				temp = {component_name(x, y-1, z+1, chunk), cur_component};
				relationships.push_back(temp);
			}
			chunk.chunk_bound(x, z, x, z-1);
			if(is_lever(x, y-1, z-1, chunk) && (chunk.return_data(x, y-1, z-1) == 4 || chunk.return_data(x, y-1, z-1) == 12))
			{
				temp = {component_name(x, y-1, z-1, chunk), cur_component};
				relationships.push_back(temp);
			}
		}
		chunk.chunk_bound(x, z, x, z);
		if(!is_dust(x, y, z, chunk) && opaque_block(x, y-1, z, chunk))
		{
			chunk.chunk_bound(x, z, x-1, z);
			if(is_dust(x-1, y-1, z, chunk))
			{
				if(is_dust(x-1, y-1, z+1, chunk) || (is_dust(x-1, y-1, z-1, chunk)))
				{
					//do nothing
				}
				else
				{			
					if((is_dust(x-1, y-2, z+1, chunk) && !opaque_block(x-1, y-1, z+1, chunk)) && (is_dust(x-1, y-2, z-1, chunk) && !opaque_block(x-1, y-1, z-1, chunk)) && ((is_dust(x-1, y, z+1, chunk) || (is_dust(x-1, y, z-1, chunk))) && !opaque_block(x-1, y, z, chunk)))
					{
						//do nothing
					}
					else
					{
						find_component_inputs(relationships, cur_component, chunk, x-1, y-1, z, offset_x-1, offset_y-1, offset_z, checked, power-1);
					}
				}
			}
			chunk.chunk_bound(x, z, x+1, z);
			if(is_dust(x+1, y-1, z, chunk))
			{
				if(is_dust(x+1, y-1, z+1, chunk) || (is_dust(x+1, y-1, z-1, chunk)))
				{
					//do nothing
				}
				else
				{			
					if((is_dust(x+1, y-2, z+1, chunk) && !opaque_block(x+1, y-1, z+1, chunk)) && (is_dust(x+1, y-2, z-1, chunk) && !opaque_block(x+1, y-1, z-1, chunk)) && ((is_dust(x+1, y, z+1, chunk) || (is_dust(x+1, y, z-1, chunk))) && !opaque_block(x+1, y, z, chunk)))
					{
						//do nothing
					}
					else
					{
						find_component_inputs(relationships, cur_component, chunk, x+1, y-1, z, offset_x+1, offset_y-1, offset_z, checked, power-1);
					}
				}
			}
			chunk.chunk_bound(x, z, x, z-1);
			if(is_dust(x, y-1, z-1, chunk))
			{
				if(is_dust(x+1, y-1, z-1, chunk) || (is_dust(x-1, y-1, z-1, chunk)))
				{
					//do nothing
				}
				else
				{			
					if((is_dust(x+1, y-2, z-1, chunk) && !opaque_block(x-1, y-1, z-1, chunk)) && (is_dust(x+1, y-2, z-1, chunk) && !opaque_block(x-1, y-1, z-1, chunk)) && ((is_dust(x+1, y, z-1, chunk) || (is_dust(x-1, y, z-1, chunk))) && !opaque_block(x+1, y, z, chunk)))
					{
						//do nothing
					}
					else
					{
						find_component_inputs(relationships, cur_component, chunk, x, y-1, z-1, offset_x, offset_y-1, offset_z-1, checked, power-1);
					}
				}
			}
			chunk.chunk_bound(x, z, x, z+1);
			if(is_dust(x, y-1, z+1, chunk))
			{
				if(is_dust(x+1, y-1, z+1, chunk) || (is_dust(x-1, y-1, z+1, chunk)))
				{
					//do nothing
				}
				else
				{			
					if((is_dust(x+1, y-2, z+1, chunk) && !opaque_block(x-1, y-1, z+1, chunk)) && (is_dust(x+1, y-2, z+1, chunk) && !opaque_block(x-1, y-1, z+1, chunk)) && ((is_dust(x+1, y, z+1, chunk) || (is_dust(x-1, y, z+1, chunk))) && !opaque_block(x+1, y, z, chunk)))
					{
						//do nothing
					}
					else
					{
						find_component_inputs(relationships, cur_component, chunk, x, y-1, z+1, offset_x, offset_y-1, offset_z+1, checked, power-1);
					}
				}
			}
		}
	}
	chunk.chunk_bound(x, z, x, z);
}

void find_component_inputs(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power)
{
	checked[offset_y][offset_z][offset_x].checked = true;
	checked[offset_y][offset_z][offset_x].chunk = chunk.return_chunk();
	
	if(is_lamp(x, y, z, chunk) || is_dust(x, y, z, chunk))
	{
		////// Check South //////
		south_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
		
		////// Check North //////
		north_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
		
		////// Check East //////
		east_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
		
		////// Check West //////
		west_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
		
		////// Check Top //////
		top_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
		
		////// Check Bottom //////
		bottom_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
	}
	else if(is_torch(x, y, z, chunk))
	{
		if(chunk.return_data(x, y, z) == 1)
		{
			west_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
		}
		if(chunk.return_data(x, y, z) == 2)
		{
			east_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
		}
		if(chunk.return_data(x, y, z) == 3)
		{
			north_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
		}
		if(chunk.return_data(x, y, z) == 4)
		{
			south_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
		}
		if(chunk.return_data(x, y, z) == 5)
		{
			bottom_check(relationships, cur_component, chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
		}
	}
	
	checked[offset_y][offset_z][offset_x].checked = false;
}

void find_component_inputs(std::vector<relationship_table>& relationships, std::string& cur_component, std::string& chunk, int x, int y, int z)
{
	bool_chunk checked[31][31][31];
	short offset_x, offset_y, offset_z;
	short power = 16;
	chunk_class cur_chunk;
	
	cur_chunk.extract_section_data(chunk);
	
	offset_x = 15;
	offset_y = 15;
	offset_z = 15;
	
	checked[offset_y][offset_z][offset_x].checked = true;
	checked[offset_y][offset_z][offset_x].chunk = cur_chunk.return_chunk();
	
	find_component_inputs(relationships, cur_component, cur_chunk, x, y, z, offset_x, offset_y, offset_z, checked, power);
}

void rm_dup_relationship(std::vector<relationship_table>& relationships)
{
	for(int i=0; i<relationships.size(); i++)
	{
		for(int j=i+1; j<relationships.size(); j++)
		{
			if((relationships[i].output.compare(relationships[j].output) == 0) && (relationships[i].input.compare(relationships[j].input)==0))
			{
				relationships.erase(relationships.begin()+j);
				j--;
			}
		}
	}
}

std::vector<relationship_table> interpret_circuit()
{
	std::string cur_component;
	int x,y,z;
	std::vector<relationship_table> relationships;
	
	// cur_component = find_active_component(chunk,x,y,z);
	
	for(int i=0; i<active_components.size(); i++)
	{
		find_component_inputs(relationships, active_components[i].label, active_components[i].chunk, active_components[i].x, active_components[i].y, active_components[i].z);	
	}
	rm_dup_relationship(relationships);
	
	// for(int i=0; i<relationships.size(); i++)
		// std::cout << relationships[i].input << " => " << relationships[i].output << std::endl;
	
	return relationships;
}
#endif