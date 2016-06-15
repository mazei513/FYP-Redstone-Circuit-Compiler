#include "interpreter.hpp"
#include "toVer.hpp"

int main()
{
	chunk_list chunks;
	chunk_class cur_chunk;
	std::vector<relationship_table> relationships;
	
	chunks.load_chunk_list();
	
	for(int i=0; i<chunks.list_size(); i++)
	{
		cur_chunk.extract_section_data(chunks.return_chunk(i));
	
		// cur_chunk.print_chunk();
		
		label_components(cur_chunk);
	}
	// print_component_labels();
	// print_active_component_labels();
	
	relationships = interpret_circuit();
	
	create_module(relationships);
	create_top_file(relationships);
	
	return 0;
}