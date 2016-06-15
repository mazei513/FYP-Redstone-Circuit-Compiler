#ifndef __tover_cpp__
#define __tover_cpp__
#include "toVer.hpp"

void rm_duplicate_string(std::vector<std::string>& str)
{
	for(int i=0; i<str.size(); i++)
	{
		for(int j=i+1; j<str.size(); j++)
		{
			if(str[i].compare(str[j]) == 0)
			{
				str.erase(str.begin()+j);
				j--;
			}
		}
	}
}

std::vector<std::string> list_inputs(std::vector<relationship_table> relationships)
{
	std::vector<std::string> inputs;
	for(int i=0; i<relationships.size(); i++)
	{
		if(relationships[i].input.find("L")!=std::string::npos)
		{
			inputs.push_back(relationships[i].input);
		}
	}
	
	rm_duplicate_string(inputs);
	
	return inputs;
}

std::vector<std::string> list_outputs(std::vector<relationship_table> relationships)
{
	std::vector<std::string> outputs;
	for(int i=0; i<relationships.size(); i++)
	{
		if(relationships[i].output.find("O")!=std::string::npos)
		{
			outputs.push_back(relationships[i].output);
		}
	}
	
	rm_duplicate_string(outputs);
	
	return outputs;
}

std::vector<std::string> list_torches(std::vector<relationship_table> relationships)
{
	std::vector<std::string> torches;
	for(int i=0; i<relationships.size(); i++)
	{
		if(relationships[i].output.find("T")!=std::string::npos)
		{
			torches.push_back(relationships[i].output);
		}
	}
	
	rm_duplicate_string(torches);
	
	return torches;
}

std::string module_header(std::vector<std::string> inputs, std::vector<std::string> outputs)
{
	std::stringstream ss;
	
	ss << "module circuit_logic (" << std::endl;
	for(int i=0; i<inputs.size(); i++)
	{
		ss << "\tinput " << inputs[i] << "," << std::endl;
	}
	for(int i=0; i<outputs.size(); i++)
	{
		ss << "\toutput " << outputs[i];
		if(i != outputs.size()-1)
			ss << "," << std::endl;
		else
			ss << std::endl;
	}
	ss << "\t);" << std::endl;
	
	return ss.str();
}

std::string create_wires(std::vector<std::string> torches)
{
	if(torches.size() > 0)
	{
		std::stringstream ss;
		
		ss << "\twire";
		for(int i=0; i<torches.size(); i++)
		{
			if(i > 0)
				ss << ",";
			ss << " " << torches[i];
		}
		ss << ";" << std::endl;
		
		return ss.str();
	}
	else
		return "";
}

std::string assign_logic(std::vector<relationship_table> relationships)
{
	std::vector<std::string> outputs = list_outputs(relationships);
	std::vector<std::string> torches = list_torches(relationships);
	std::stringstream ss;
	
	for(int i=0; i<outputs.size(); i++)
	{
		bool first = true;
		ss << "\tassign " << outputs[i] << " = ";
		for(int j=0; j<relationships.size(); j++)
		{
			if(relationships[j].output.compare(outputs[i]) == 0)
			{
				if(first)
				{
					ss << relationships[j].input;
					first = false;
				}
				else
					ss << " | " << relationships[j].input;
			}
		}
		ss << ";" << std::endl;
	}
	for(int i=0; i<torches.size(); i++)
	{
		bool first = true;
		ss << "\tassign " << torches[i] << " = ~(";
		for(int j=0; j<relationships.size(); j++)
		{
			if(relationships[j].output.compare(torches[i]) == 0)
			{
				if(first)
				{
					ss << relationships[j].input;
					first = false;
				}
				else
					ss << " | " << relationships[j].input;
			}
		}
		ss << ");" << std::endl;
	}
	
	return ss.str();
}

void create_module(std::vector<relationship_table> relationships)
{
	std::string header = module_header(list_inputs(relationships), list_outputs(relationships));
	std::string wires = create_wires(list_torches(relationships));
	std::string logic = assign_logic(relationships);
	std::string module_end = "endmodule";
	std::ofstream os;
	os.open("QuartusTemplate/circuit_logic.v", std::ios::trunc);
	
	std::cout << header << wires << logic << module_end << std::endl;
	os << header << wires << logic << module_end;
	
	os.close();
}

void create_top_file(std::vector<relationship_table> relationships)
{
	std::string header = "module top_module (\n\tSW,\n\tLEDG\n\t);\n";
	std::string sw = "\tinput [9:0] SW;\n";
	std::string led = "\toutput [9:0] LEDG;\n";
	std::string end = "endmodule";
	std::string module = "\tcircuit_logic SEG0 (";
	std::vector<std::string> inputs = list_inputs(relationships);
	std::vector<std::string> outputs = list_outputs(relationships);
	std::stringstream ss;
	std::ofstream os;
	bool first = true;
	
	for(int i=0; i<inputs.size(); i++)
	{
		if(!first)
		{
			ss << ", ";
		}
		ss << "SW[" << i << "]";
		first = false;
	}
	for(int i=0; i<outputs.size(); i++)
	{
		ss << ", LEDG[" << i << "]";
	}
	ss << ");" << std::endl;
	
	module += ss.str();
	
	os.open("QuartusTemplate/top_module.v", std::ios::trunc);
	
	std::cout << header << sw << led << module << end << std::endl;
	os << header << sw << led << module << end;
	
	os.close();
}

// int main()
// {
	// std::vector<relationship_table> relationships;
	// relationship_table temp;
	
	// temp = {"L0", "O2"};
	// relationships.push_back(temp);
	
	// temp = {"T1", "O1"};
	// relationships.push_back(temp);
	
	// temp = {"T1", "O2"};
	// relationships.push_back(temp);
	
	// temp = {"L1", "O2"};
	// relationships.push_back(temp);
	
	// temp = {"L3", "T1"};
	// relationships.push_back(temp);
	
	// create_module(relationships);
	
	// return 0;
// }
#endif