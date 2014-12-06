// Amy Ngai
// Has been tested at all
// psuedo code for comparing a sample from a bank

module compareFromBank(
                  input  logic[2:0] state,
                  input  logic	clk,
					//	input  byte audio[1999:0],
					output logic resultCompare,
					output logic finalIndex,
					output logic transmit_ready);
	
											
	// Load audiobank from ROM
	logic [9:0] audiobank[3999:0]; // Assumes we have 4 audio samples
								 // 2000 8-bit samples each

	// Identify each audio sample
	logic[9:0] cat[1999:0], stop[1999:0], audio[1999:0];
	
	// The result of the correlation
	logic[35:0] maxResultCat, maxresultStop;
	logic[11:0] maxIndexCat, maxIndexStop;
					
	logic finishedCat, finishedStop;
	
	initial
			$readmemh("audioBankRom.txt", audiobank); 
			
	assign cat = audiobank[1999:0];
	assign stop = audiobank[3999:2000];
	assign audio = cat;

	//assign on = '{audiobank[5999:4000]};
	//assign off = '{audiobank[7999:6000]};
			

	correlate corelateCat(state, clk, audio, cat, finishedCat, maxResultCat, maxIndexCat);
	correlate correlateOpen(state, clk, audio, stop, finishedStop, maxresultStop, maxIndexStop);
	
	always_comb begin
		if (maxresultStop > maxResultCat) begin
			resultCompare = 1'b1;
			finalIndex = maxIndexStop;
		end else begin
			resultCompare = 1'b0;
			finalIndex	= maxIndexCat;
		end
	end

	assign transmit_ready = (finishedCat & finishedStop );


endmodule
