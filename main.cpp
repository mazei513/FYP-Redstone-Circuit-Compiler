#include "interpreter.hpp"

int main()
{
	chunk_list chunks;
	chunk_class cur_chunk;
	
	chunks.load_chunk_list();
	
	cur_chunk.extract_section_data(chunks.return_chunk(0));
	
	cur_chunk.print_chunk();
	
	label_components(cur_chunk);
	interpret_circuit(cur_chunk);
	
	return 0;
}