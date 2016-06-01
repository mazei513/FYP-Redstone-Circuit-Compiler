#include "interpreter.hpp"

int main()
{
	chunk_list chunks;
	chunk_class cur_chunk;
	
	chunks.load_chunk_list();
	
	for(int i=0; i<chunks.list_size(); i++)
	{
		cur_chunk.extract_section_data(chunks.return_chunk(i));
	
		// cur_chunk.print_chunk();
		
		label_components(cur_chunk);
	}
	// print_component_labels();
	
	interpret_circuit();
	
	return 0;
}