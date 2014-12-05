// Amy Ngai
// Has been tested at all
// psuedo code for comparing a sample from a bank

module compareFromBank(
                  input  logic[2:0] state,
                  input  logic	clk,
						input  byte audio[1999:0],
						output logic [3:0] result,
						output logic transmit_ready);
	
											
	// Load audiobank from ROM
	byte audiobank[7999:0]; // Assumes we have 4 audio samples
								 // 2000 8-bit samples each

	// Identify each audio sample
	byte open[1999:0],close[1999:0],on[1999:0], off[1999:0];
	
	// The result of the correlation
	byte resultOpen[3998:0],resultClose[3998:0],
					resultOn[3998:0],resultOff[3998:0];
					
	logic openFin, closeFin, onFin, offFin;
	initial
			$readmemb("audiobBankRom.txt", audiobank); 
			
	//assign open = '{audiobank[1999:0]};
	//assign close = '{audiobank[3999:2000]};
	//assign on = '{audiobank[5999:4000]};
	//assign off = '{audiobank[7999:6000]};
	assign open = '{2000{8'b1}};
	assign close = open;
	assign on = open;
	assign off = open;
			
	correlate correlateOpen(state, clk, audio, open, resultOpen, openFin);
	correlate correlateClose(state, clk, audio, close, resultClose, closeFin);
	correlate corelateOn(state, clk, audio, on, resultOn, onFin);
	correlate correlateOff(state, clk, audio, off, resultOff, offFin);
	
	assign transmit_ready = ((state == 3'b010) & openFin & closeFin & onFin & offFin);
	
	assign result = 4'b1010; // FIXME

endmodule
