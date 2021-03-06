// Amy Ngai
// mngai@hmc.edu
// Fall 2014
//
// This module calculates the correlation between two input signals.
// It assumes that the inputs are 4 32-bit integers.
// The output is a 7 32-bit integers.


// assuming everything can be written as arrays
module correlate(input  logic[2:0] topState,
				input logic		clk,
				input logic[9:0]	a[3:0], b[3:0],
				//output logic[9:0] 	result[6:0],
				output logic 	finished,
				output logic [9:0] maxResult,
				output logic [11:0] maxIndex);

	typedef enum logic [1:0] {WAITING = 2'b10, PROCESSING=2'b00, DONE=2'b01} statetype;
	statetype currentState, nextState;

	logic [9:0] intA, intB;	// look at one integer at a time 
	logic [11:0] lengthResult;
	logic [11:0] lengthA;
	logic assignA, assignB; // flags if you should assign intA and intB

	logic [9:0] currentResult;
	
	assign lengthResult = 12'd7;
	assign lengthA = 12'd4;
	
	logic [11:0] n, nextN;	// iterate through entire output signal
	logic [11:0] k, nextK;	// iterate through one of the signal

	// State register
    always_ff @(posedge clk) begin
		currentState <= nextState;
		n <= nextN;
		k <= nextK;
	end

	// Next state logic
    always_comb begin

    	// begin index logic 
    	if ((topState == 3'b010) && (currentState == PROCESSING)) begin

	 		if (n >= (lengthResult)) nextN = lengthResult;
	 		else nextN = (k == n)? (n + 12'd1): n;

	 		if (k == n) nextK = 12'b0;
	 		else nextK = k + 12'b1;

	 	end else if (topState == 3'b000) begin
	 		nextN = 12'd0;
	 		nextK = 12'd0;
	 	end else begin
	 		nextN = n;
	 		nextN = k;
	 	end


	 	// begin nextState logic
        case(currentState)
        	WAITING: 	nextState = (topState == 3'b010)? PROCESSING : WAITING;
            PROCESSING: nextState = (n >= lengthResult)? DONE : PROCESSING; // Wait until counter 
            DONE: 		nextState = DONE; // S1: Done processing
            default: nextState = WAITING;
        endcase  


        // convolution logic
        assignA = (n-k) > (lengthA - 12'd1);
        assignB = lengthA - 12'd1 < k;
        


		if ((topState == 3'b010) && (currentState == PROCESSING)) begin // where the convolution magic happens

			intA = (assignA)? 10'b0: a[n-k];
			intB = (assignB)? 10'b0: b[lengthA - k - 1];

			currentResult = (k == 0)? intA*intB : currentResult + intA*intB;
		

			if (maxResult < currentResult) begin
				maxResult = currentResult;
				maxIndex = n;
			end 

		end else if (topState == 3'b000) begin
			intA = 10'b0;
			intB = 10'b0;
			maxResult = 10'd0;
			maxIndex = 12'd0;
			currentResult = 10'd0;

		end else begin
			intA = 10'b0;
			intB = 10'b0;
		end



 	end
   
	 
assign finished = (currentState == DONE);



endmodule