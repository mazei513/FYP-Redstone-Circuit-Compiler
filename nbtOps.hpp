#ifndef __nbtops_hpp__
#define __nbtops_hpp__
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <sstream>

const std::string NBTFILEPATH = "chunks\\";

class chunk_list
{
	private:
		std::vector<std::string> chunks;
	public:
		chunk_list(){}
		~chunk_list(){}
		void load_chunk_list();
		std::string return_chunk(int n) {return chunks[n];}
		int list_size() {return chunks.size();}
};

class chunk_class
{
	private:
		std::string chunk_path;
		std::string chunk;
		unsigned short blocks[16][16][16];
		unsigned short data[16][16][16];
		int x_pos;
		int z_pos;
		int y_pos;
		bool open_chunk_file(std::string chunk, std::ifstream& in_file);
	public:
		chunk_class(){}
		~chunk_class(){}
		unsigned short return_block(short x, short y, short z){return blocks[y][z][x];}
		unsigned short return_data(short x, short y, short z){return data[y][z][x];}
		int return_xPos(){return x_pos;}
		int return_zPos(){return z_pos;}
		std::string return_chunk(){return chunk;}
		void extract_section_data(std::string chunk_name);
		// void extract_block_data(std::string current_file);
		void print_chunk();
};
#endif