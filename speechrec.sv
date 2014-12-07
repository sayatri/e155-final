// speechrec.sv
// 16 November 11
// strisorus@hmc.edu
// 
// Receives an audio signal via SPI and compares it to audio signals stored in memory

// sdi with respect to slave
// sdo with respect to slave
module speechrec(input  logic       clk, sck, sdi, ss, reset, 
                 output logic       sdo,
                 output logic [7:0] led,
					  output logic       transmit_ready,
					  output logic       receive_ready,
					  output logic       data_confirmed); //TODO

    // Extra signals
	 logic data_exchanged;  // Keep track of whether or not all data has been exchanged via SPI
    logic input_ready;     // Keep track of whether or not input audio has been received and
                           //   processing is ready to begin
	 logic transmit_done;   // Keep track of whether or not transmission of data to the master
	                        //   has been completed
	// from compile audio								
	logic [10:0] cnt;

    logic [31:0] data_send, data_receive, junk_send, junk_receive; // Data logic
    logic [9:0]  audio[1999:0];                                    // Received audio data
    //logic         enable_processing;
    //logic           complete_send;
    

    // State signals
    typedef enum logic [3:0] {S0=4'b111, S1=4'b001, S2=4'b010, S3=4'b011, S4=4'b100, S5=4'b101, S6=4'b110, S7 = 4'b000} statetype;
    statetype state, nextstate;

    // State register
    always_ff @(posedge clk)
        state = (reset)? S0: nextstate;

    // Next state logic
    always_comb begin
        case(state)
            S0: nextstate =      	    ss ? S1 : S0; // S0: Wait for master to send input audio
            S1: nextstate = 		  input_ready ? S3 : S1;	//input_ready ? S3 : S1; // S1: Receive the audio
            S2: nextstate = transmit_ready ? S3 : S2; // S2: Process the audio
            S3: nextstate =             ss ? S4 : S3; // S3: Wait for master to be ready
            S4: nextstate = 				~ss ? S5 : S4; // S4: Transmit the results data
				S5: nextstate =             ss ? S6 : S5; // S5: Wait for the master to be ready
				S6: nextstate = data_exchanged ? S7 : S6; // S6: Receive the transmitted data back
				S7: nextstate = data_confirmed ? S0 : S3; // S7: Confirm master received correct data
            default: nextstate = S0;
       endcase
    end
    
	 assign led[3:0] = state;
	 assign led[7:4] = cnt[4:0];
	 
   // spi_send_receive receiver(state, sck, sdo, sdi, reset, junk_send, data_receive, input_ready);
	// spi_send_receive receiver(state, sck, sdi, sdo, reset, data_send, data_receive);
	spi_send_receive comm(state, sck, sdi, sdo, data_send, data_receive, data_exchanged);
	compile_audio compiler(clk, state, data_receive, data_exchanged, audio, input_ready, cnt);
	compare_signals checker(state, data_send, compare_receive, data_confirmed);
	
	// TODO: Temporarily always be ready to transmit on the state
	assign transmit_ready = (state == S3);//input_ready;
	
	// Ready to receive in reception states
	assign receive_ready = (state == S1 | state == S4 | state == S6);
	
	// TODO: Temporarily transmit a 10-bit logic
	logic [9:0] res;
	assign res = 10'b1010101010;
	assign data_send = 32'h00001111;
  assign compare_receive = ((state == S6) && data_exchanged)? data_received : 32'b0;

	
    //assign led = data_receive;
     // FIXME vvv
     //store store_int(state, sck, data_receive, input_ready, audio);
     // FIXME ^^^
     
    //spi_send_receive transmitter(state, sck, sdo, sdi, reset, data_send, junk_receive, transmit_done);
    //compareFromBank compareFromBank_inst(state, clk, audio, result, transmit_ready);

endmodule

module compare_signals(input  logic [3:0]  state,
                       input  logic [31:0] sent,
                       input  logic [31:0] received,
							  output logic        data_confirmed);
  always_comb begin
    if (state == 4'b000)  data_confirmed = (sent == received);
	 else                  data_confirmed = 0;
  end
							  
endmodule

module compile_audio(input  logic        clk,
                     input  logic [3:0]  state,
                     input  logic [31:0] data,
                     input  logic        data_exchanged,
                     output logic [9:0]  audio[1999:0],
						   output logic        input_ready,
							output logic [10:0] cnt);

 // logic [10:0] cnt;

							
  always_ff @(posedge clk) begin
    // Reset the counter if new data is going to be received
    if (state == 4'b111) begin
	   cnt <= 0;
	 end

	 // Add to the audio array until all data has been added
    else if ((state == 4'b001) && data_exchanged && cnt < 11'd1000) begin
	   audio[cnt] <= data[9:0];
		cnt <= cnt + 11'b1;
	 end
	 
	 // Set the flag when all the data has been added
	 input_ready <= (cnt >= 11'd1000 &&(state !=4'b111));// && state == 3'b001);
  end
endmodule	
                     

module spi_send_receive(input  logic [3:0]  state,
                        input  logic        sck, sdi,  // from master
                        output  logic       sdo,   // from master 
	     				     	input  logic [31:0] d,	// data send
                        output logic [31:0] q,     // data received
								output logic        data_exchanged); // state flag
						 
  logic [4:0]  cnt;
  logic [31:0] buffer;
  logic        qdelayed;
  
  always_ff @(negedge sck) begin	 
    // set flag and store data after data fully exchanged
	 if ((state == 4'b001 || state == 4'b110) && cnt >= 5'd31) begin
	   q   <= buffer;
	 end
	 
	 // increase count if in a state where the module is active
	 // reset count if in a waiting state
	 if (state == 4'b001 || state == 4'b100) begin
	   cnt <= cnt + 5'b1;
	 end
	 else if (state == 4'b111 || state == 4'b011 || state == 4'b101) begin
	   cnt <= 0;
    end
  end
  
  always_comb begin
	 data_exchanged = ((cnt >= 5'd31) & (state == 4'b001 || state == 4'b100 || state == 4'b110));
  end
	
  always_ff @(posedge sck)
    buffer <= (cnt == 0) ? d : {buffer[30:0], sdi}; //shift register

  always_ff @(negedge sck)
    qdelayed <= buffer[31];

  assign sdo = (cnt == 0) ? d[31] : qdelayed;
endmodule
