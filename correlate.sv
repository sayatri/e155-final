// Amy Ngai
// mngai@hmc.edu
// Fall 2014
//
// This module calculates the correlation between two input signals.
// It assumes that the inputs are 4 32-bit integers.
// The output is a 7 32-bit integers.


// assuming everything can be written as arrays
module correlate(input  logic[2:0] state,
				input logic		clk,
				input logic[9:0]	a[1999:0], b[1999:0],
				output logic[9:0] 	result[3998:0],
				output logic 	finished);

	typedef enum logic {PROCESSING=1'b0, DONE=1'b1} statetype;
   statetype currentState, nextstate;

	byte intA, intB;	// look at one integer at a time 
	logic[31:0] lengthResult, lengthA;
	
	assign lengthResult = 32'd3999;
	assign lengthA = 32'd1999;
	
	logic [3:0] n;	// FIXME
	logic [3:0] k;	// FIXME



	// State register
    always_ff @(posedge clk) begin
	 
		// only continue if you are in the processing state from
		// the top module
		if (state == 3'b010) begin
			  currentState <= nextstate;

			  // for n=0:(2*length(a) - 1) -1
				if (n >= (lengthResult - 1)) n <= lengthResult;
				else n <= (k == n)? (n + 1): n;
				
				// for k=1:n
				if (k == n) k = 0;
				else k = k + 1;

				if (state == PROCESSING) begin // where the convolution magic happens
					intA <= ((n-k) > 3)? 8'b0: {a[(n-k)]};

					intB <= (lengthA - k < 0)? 8'b0: {b[lengthA - k]};

					result[n] <= result[n] + intA*intB;

				end else begin
					intA <= 8'b0;
					intB <= 8'b0;
					result <= result;
				end
		
		end else begin
			n <= lengthResult;
			k <= 4'b0;
			intA <= 8'b0;
			intB <= 8'b0;
			result <= '{3999{8'b0}};
		end
	end

    // Next state logic
    always_comb begin
        case(currentState)
            PROCESSING: nextstate = (n >= lengthResult)? DONE : PROCESSING; // Wait until counter 
            DONE: 		nextstate = DONE; // S1: Done processing
            default: nextstate = DONE;
        endcase
    end
	 
	 assign finished = (currentState == DONE);



endmodule