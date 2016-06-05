#ifndef __tover_hpp__
#define __tover_hpp__
#include <string>
#include <iostream>
#include "interpreter.hpp"
#include <vector>
#include <sstream>

void rm_duplicate_string(std::vector<std::string>& str);
std::vector<std::string> list_inputs(std::vector<relationship_table> relationships);
std::vector<std::string> list_outputs(std::vector<relationship_table> relationships);
std::vector<std::string> list_torches(std::vector<relationship_table> relationships);
std::string module_header(std::vector<std::string> inputs, std::vector<std::string> outputs);
std::string create_wires(std::vector<std::string> torches);
std::string assign_logic(std::vector<relationship_table> relationships);
void create_module(std::vector<relationship_table> relationships);
#endif