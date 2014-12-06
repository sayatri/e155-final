// Amy Ngai
// Has been tested at all
// psuedo code for comparing a sample from a bank

module compareFromBankSmall(
                  		input  logic[2:0] state,
                		input  logic	clk,
					//	input  logic[9:0] audio[1999:0],
						output logic resultCompare,
						output logic finalIndex,
						output logic transmit_ready);
											
	// Load audiobank from ROM
	logic[9:0] audiobank[21:0]; // Assumes we have 4 audio samples
								 // 2000 8-bit samples each

	//logic[9:0] open[1999:0],close[1999:0],on[1999:0], off[1999:0];
	logic [9:0] a[3:0], b[3:0];
 //  	logic [9:0] resultAB[6:0], correctResultAB[6:0];
 //  	logic [9:0] resultAA[6:0], correctResultAA[6:0];

	logic [36:0] maxResultAB, maxResultAA;
	logic [11:0] maxIndexAB, maxIndexAA;

	logic finishedAB, finishedAA;

//	logic openFin;
	// The result of the correlation
	//logic[9:0] resultOpen[3998:0],resultClose[3998:0],
	//				resultOn[3998:0],resultOff[3998:0];
					
	// logic openFin, closeFin, onFin, offFin;
	initial
			$readmemh("testSmallCorrelation.txt", audiobank); 
		
	// Identify each audio sample		
	assign a = audiobank[3:0];
	assign b = audiobank[7:4];
//	assign correctResultAB = audiobank[14:8];
//	assign correctResultAA = audiobank[21:15];

			
	correlate correlateSmallAB(state, clk, a, b, finishedAB, maxResultAB, maxIndexAB);
	correlate correlateSmallAA(state, clk, a, a, finishedAA, maxResultAA, maxIndexAA);



	always_comb begin
		if (maxResultAB > maxResultAA) begin
			resultCompare = 1'b1;
			finalIndex = maxIndexAB;
		end else begin
			resultCompare = 1'b0;
			finalIndex = maxIndexAA;
		end
	end

	assign transmit_ready = (finishedAB && finishedAA);

endmodule
