// Amy Ngai
// mngai@hmc.edu
// Fall 2014
//
// This module calculates the correlation between two input signals.
// It assumes that the inputs are 4 32-bit integers.
// The output is a 7 32-bit integers.


// assuming everything can be written as arrays
module correlate(input logic		clk,
				input int	a[3:0],
				input int	b[3:0],
				output int	result[7:0]);

	typedef enum logic {PROCESSING=1'b0, DONE=1'b1} statetype;
   statetype state, nextstate;

    // enum logic LENGTH_A=4;

	int intA, intB;	// look at one integer at a time 
	logic[2:0] lengthResult, lengthA;
	
	assign lengthResult = 2'd7;
	assign lengthA = 2'd4;
	
	/// maybe this line of code???
	// int intA, intB
	/// maybe....
	logic [3:0] n;	// for n = 1
	logic [3:0] k;	// for the second term



	// State register
    always_ff @(posedge clk) begin
        state <= nextstate;

        // for n=1:(2*length(a) - 1) 
       	if (n >= lengthResult) n <= lengthResult;
       	else n <= (k == n)? n + 1: n;
       	
       	// for k=1:n
       	if (k == n) k = 0;
       	else k = k + 1;

       	if (state == PROCESSING) begin // where the convolution magic happens
       		intA <= ((n-k) > 3)? 32'b0: {a[(n-k)]};

				intB <= (lengthA - k < 0)? 32'b0: {b[lengthA - k]};

				result[n] <= result[n] + intA*intB;

       	end else begin
       		intA <= 32'b0;
       		intB <= 32'b0;
       		result <= result;
       	end
		end

    // Next state logic
    always_comb begin
        case(state)
            PROCESSING: nextstate = (n >= lengthResult)? DONE : PROCESSING; // Wait until counter 
            DONE: 		nextstate = DONE; // S1: Done processing
            default: nextstate = DONE;
        endcase
    end



endmodule