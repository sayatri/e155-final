module store (input logic sck,
					input logic[2:0] state,
					input [7:0] data_receive,
					input input_ready, 
					output byte audio[1999:0]);
	
	initial
		$readme("fake.txt", audio);

endmodule