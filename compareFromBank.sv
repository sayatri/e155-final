//// Amy Ngai
//// Has been tested at all
//// psuedo code for comparing a sample from a bank
//
//module compareFromBank(input logic	clk,
//						input logic [7:0] audio[1999:0],
//						output logic [3:0] result)
//	
//	// Load audiobank from ROM
//	logic[7:0] audiobank[7999:0]; // Assumes we have 4 audio samples
//								 // 2000 8-bit samples each
//	initial $readmemb("audiobank", audiobank); 			
//
//
//	// Identify each audio sample
//	logic[7:0] open[1999:0];
//	logic[7:0] close[1999:0];
//	logic[7:0] on[1999:0];
//	logic[7:0] off[1999:0];
//
//	assign open = {audiobank[0]};
//	assign close = {audiobank[2000]};
//	assign on = {audiobank[4000]};
//	assign off = {audiobank[6000]};
//
//	correlate correlateOpen(clk, audio, open, resultOpen);
//	correlate correlateClose(clk, audio, close, resultClose);
//	correlate corelateOn(clk, audio, On, resultOn);
//	correlate correlateOff(clk, audio, Off, resultOff);
//
//	// TODO find max of the four
//	// return whichever is max
//
//endmodule
