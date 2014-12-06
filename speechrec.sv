// speechrec.sv
// 16 November 11
// strisorus@hmc.edu
// 
// Receives an audio signal via SPI and compares it to audio signals stored in memory

module speechrec(input  logic       clk, sck, sdi, ss, reset,
                 output logic       sdo,
                 output logic [7:0] led,
					  output logic [3:0] result); //TODO

    // Extra signals
	 logic data_exchanged;  // Keep track of whether or not all data has been exchanged via SPI
    logic input_ready;     // Keep track of whether or not input audio has been received and
                           //   processing is ready to begin
    logic transmit_ready;  // Keep track of whether or not processing is complete and data
                           //   is ready to be sent back to master
	 logic transmit_done;   // Keep track of whether or not transmission of data to the master
	                        //   has been completed

    logic [31:0] data_send, data_receive, junk_send, junk_receive; // Data logic
    logic [9:0]  audio[1999:0];                                    // Received audio data
    //logic         enable_processing;
    //logic           complete_send;
    

    // State signals
    typedef enum logic [2:0] {S0=3'b111, S1=3'b001, S2=3'b010, S3=3'b011, S4=3'b100} statetype;
    statetype state, nextstate;

    // State register
    always_ff @(posedge clk)
        state <= nextstate;

    // Next state logic
    always_comb begin
        case(state)
            S0: nextstate =             ss ? S1 : S0; // S0: Wait for master to send input audio
            S1: nextstate =    input_ready ? S3 : S1; // S1: Receive the audio
            S2: nextstate = transmit_ready ? S3 : S2; // S2: Process the audio
            S3: nextstate =             ss ? S4 : S3; // S3: Wait for master to be ready
            S4: nextstate = data_exchanged ? S0 : S4; // S4: Transmit the audio
            default: nextstate = S0;
       endcase
    end
    
	 assign led = state;
	 
   // spi_send_receive receiver(state, sck, sdo, sdi, reset, junk_send, data_receive, input_ready);
	// spi_send_receive receiver(state, sck, sdi, sdo, reset, data_send, data_receive);
	spi_send_receive comm(state, sck, sdi, sdo, data_send, data_receive, data_exchanged);
	compile_audio compiler(clk, state, data_receive, data_exchanged, audio, input_ready);
	
	// TODO: Temporarily transmit a 10-bit logic
	logic [9:0] res;
	assign res = 10'b1010101010;
	assign data_send = {22'b0, res};
	
    //assign led = data_receive;
     // FIXME vvv
     //store store_int(state, sck, data_receive, input_ready, audio);
     // FIXME ^^^
     
    //spi_send_receive transmitter(state, sck, sdo, sdi, reset, data_send, junk_receive, transmit_done);
    //compareFromBank compareFromBank_inst(state, clk, audio, result, transmit_ready);

endmodule

module compile_audio(input  logic        clk,
                     input  logic [2:0]  state,
                     input  logic [31:0] data,
                     input  logic        data_exchanged,
                     output logic [9:0]  audio[1999:0],
						   output logic        input_ready);

  logic [10:0] cnt;
							
  always_ff @(posedge clk) begin
    // Reset the counter if new data is going to be received
    if (state == 3'b111) begin
	   cnt <= 0;
	 end

	 // Add to the audio array until all data has been added
    else if (data_exchanged && cnt < 11'd2000) begin
	   audio[cnt] <= data[9:0];
		cnt <= cnt + 11'b1;
	 end
	 
	 // Set the flag when all the data has been added
	 input_ready <= (cnt >= 11'd2000 && state == 3'b001);
  end
endmodule	
                     

module spi_send_receive(input  logic [2:0]  state,
                        input  logic        sck, sdi,  // from master
                        output  logic       sdo,   // from master 
	     				     	input  logic [31:0] d,
                        output logic [31:0] q,     // data received
								output logic        flag); // state flag
						 
  logic [4:0]  cnt;
  logic [31:0] buffer;
  logic        qdelayed;
  
  always_ff @(negedge sck) begin	 
    // set flag and store data after data fully exchanged
	 if (state == 3'b001 && cnt >= 5'd31) begin
	   q   <= buffer;
	 end
	 
	 // increase count if in a state where the module is active
	 // reset count if in a waiting state
	 if (state == 3'b001 || state == 3'b100) begin
	   cnt <= cnt + 5'b1;
	 end
	 else if (state == 3'b111 || state == 3'b011) begin
	   cnt <= 0;
    end
  end
  
  always_comb begin
	 flag = ((cnt >= 5'd31) & (state == 3'b001 || state == 3'b100));
  end
	
  always_ff @(posedge sck)
    buffer <= (cnt == 0) ? d : {buffer[30:0], sdi}; //shift register

  always_ff @(negedge sck)
    qdelayed <= buffer[31];

  assign sdo = (cnt == 0) ? d[31] : qdelayed;
endmodule
