#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

struct comp_pos_label
{
	int x;
	int y;
	int z;
	std::string label;
	std::string chunk;
};

struct relationship_table
{
	std::string input;
	std::string output;
};

int current_lever_n = 1;
int current_torch_n = 1;
int current_lamp_n = 1;
std::vector<comp_pos_label> component_labels;
std::string chunk_name = "r.txt";

const int REDSTONELAMP_ID = 123;
const int LEVER_ID = 69;
const int REDSTONEDUST_ID = 55;

void read_chunk_file(int read_chunk[][16][16]);
void print_chunk(int read_chunk[][16][16]);
std::string cur_lever_label();
std::string cur_torch_label();
std::string cur_lamp_label();
void label_components(int read_chunk[][16][16]);
void print_component_labels();
std::string component_name(int x, int y, int z);
std::string find_active_component(int read_chunk[][16][16], int &x, int &y, int &z);
void component_switch_case(std::vector<relationship_table> &relationships, std::string cur_component, int read_chunk[][16][16], int x, int y, int z, bool checked[][16][16]);
void find_component_inputs(std::vector<relationship_table> &relationships, std::string cur_component, int read_chunk[][16][16], int x, int y, int z, bool checked[][16][16]);
void find_component_inputs(std::vector<relationship_table> &relationships, std::string cur_component, int read_chunk[][16][16], int x, int y, int z);
void interpret_circuit(int read_chunk[][16][16]);
