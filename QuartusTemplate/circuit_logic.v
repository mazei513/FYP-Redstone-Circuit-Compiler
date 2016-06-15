module circuit_logic (
	input L1,
	input L2,
	input L3,
	input L4,
	output O1
	);
	wire T1, T2, T3, T4;
	assign O1 = T2 | T1;
	assign T1 = ~(L1 | L2);
	assign T2 = ~(T4 | T3);
	assign T3 = ~(L3);
	assign T4 = ~(L4);
endmodule