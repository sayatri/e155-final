// Amy Ngai
// Has been tested at all
// psuedo code for comparing a sample from a bank

module compareFromBank(
                  	input  logic[2:0] state,
                  	input  logic	clk,
					output logic[14:0] resultOne, resultTwo,
					output logic[11:0] maxIndexOne, maxIndexTwo,
					output logic transmit_ready);
	
											
	// Load audiobank from ROM
	logic [9:0] audiobank[3999:0]; // Assumes we have 4 audio samples
								 // 2000 8-bit samples each

	// Identify each audio sample
	logic[9:0] One[1999:0], Two[1999:0], audio[1999:0];
	
	// The result of the correlation
	logic[35:0] maxResultOne, maxResultTwo; // cat
					
	logic finishedOne, finishedTwo;
	
	initial
		$readmemh("audioBankRom.txt", audiobank); 
			
	assign One = audiobank[1999:0];	// Cat
	assign Two = audiobank[3999:2000];	// open
	assign audio = One;

	//assign on = '{audiobank[5999:4000]};
	//assign off = '{audiobank[7999:6000]};
			

	correlate corelateOne(state, clk, audio, One, finishedOne, maxResultOne, maxIndexOne);
	correlate correlateOpen(state, clk, audio, Two, finishedTwo, maxresultTwo, maxIndexTwo);
	
	assign transmit_ready = (finishedOne & finishedTwo );

	assign resultOne = maxResultOne[35:21];
	assign resultTwo = maxResultTwo[35:21];


endmodule
