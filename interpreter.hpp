#ifndef __interpreter_hpp__
#define __interpreter_hpp__
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "nbtOps.hpp"

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

struct bool_chunk
{
	bool checked;
	std::string chunk;
	bool_chunk():checked(false),chunk("") { }
};

const int REDSTONELAMP_ID = 123;
const int REDSTONETORCH_ID = 75;
const int LEVER_ID = 69;
const int REDSTONEDUST_ID = 55;
const int AIR_BLOCK_ID = 0;


std::string cur_lever_label();
std::string cur_torch_label();
std::string cur_lamp_label();
void label_components(chunk_class chunk);
void print_component_labels();
std::string component_name(int x, int y, int z, chunk_class chunk);
std::string find_active_component(chunk_class chunk, int &x, int &y, int &z);
bool air_block(int x, int y, int z, chunk_class chunk);
bool transparent_block(int x, int y, int z, chunk_class chunk);
bool component(int x, int y, int z, chunk_class chunk);
bool opaque_block(int x, int y, int z, chunk_class chunk);
void south_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power);
void north_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power);
void east_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power);
void west_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power);
void top_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power);
void bottom_check(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power);
void find_component_inputs(std::vector<relationship_table>& relationships, std::string& cur_component, chunk_class& chunk, int x, int y, int z, short offset_x, short offset_y, short offset_z, bool_chunk checked[][31][31], short power);
void find_component_inputs(std::vector<relationship_table>& relationships, std::string& cur_component, std::string& chunk, int x, int y, int z);
void rm_dup_relationship(std::vector<relationship_table>& relationships);
std::vector<relationship_table> interpret_circuit();
#endif