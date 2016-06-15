module top_module (
	SW,
	LEDG
	);
	input [9:0] SW;
	output [9:0] LEDG;
	circuit_logic SEG0 (SW[0], SW[1], SW[2], SW[3], LEDG[0]);
endmodule