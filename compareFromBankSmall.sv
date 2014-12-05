// Amy Ngai
// Has been tested at all
// psuedo code for comparing a sample from a bank

module compareFromBankSmall(
                  input  logic[2:0] state,
                  input  logic	clk,
					//	input  logic[9:0] audio[1999:0],
						output logic resultCompare,
						output logic openFin);
	
											
	// Load audiobank from ROM
	logic[9:0] audiobank[14:0]; // Assumes we have 4 audio samples
								 // 2000 8-bit samples each

	// Identify each audio sample
	//logic[9:0] open[1999:0],close[1999:0],on[1999:0], off[1999:0];
	logic [9:0] a[3:0], b[3:0];
   logic [9:0] result[6:0], correctResult[6:0];
	
//	logic openFin;
	// The result of the correlation
	//logic[9:0] resultOpen[3998:0],resultClose[3998:0],
	//				resultOn[3998:0],resultOff[3998:0];
					
	// logic openFin, closeFin, onFin, offFin;
	initial
			$readmemh("testSmallCorrelation.txt", audiobank); 
			
	assign a = audiobank[3:0];
	assign b = audiobank[7:4];
	assign correctResult = audiobank[14:8];

			
	correlate correlateSmall(state, clk, a, b, result, openFin);

	
	assign resultCompare = (openFin & (result == correctResult));
endmodule
