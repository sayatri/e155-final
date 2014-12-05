// speechrec.sv
// 16 November 11
// strisorus@hmc.edu
// 
// Receives an audio signal via SPI and compares it to audio signals stored in memory

module speechrec(input  logic       clk, sck, sdi, //ss,
                 output logic       sdo,
                 output logic [7:0] led);

    // Extra signals
    logic input_ready;     // Keep track of whether or not input audio has been
                           // received and processing is ready to begin
    logic transmit_ready;  // Keep track of whether or not processing is complete
                           // and data is ready to be sent back to master
    logic [7:0]     data;  // Temporary data to send to master
    //logic [39999:0] audio; // Input audio data
    logic [7:0]     audio; // TODO: Received data for testing
    //logic [15:0]    count; // Keep count of bits of data received
    logic [3:0]     count; // TODO: Shortened counter for testing
	 logic [7:0] data_copy;
	 logic 		enable_processing;
	 logic 		complete_send;

    // State signals
    typedef enum logic [2:0] {S0=3'b000, S1=3'b001, S2=3'b010, S3=3'b011, S4=3'b100} statetype;
    statetype state, nextstate;

    // State register
    always_ff @(posedge clk)
        state <= nextstate;

    // Next state logic
    always_comb begin
        case(state)
            S0: nextstate =             ss ? S1 : S0; // S0: Wait for master to send input audio
            S1: nextstate =    input_ready ? S2 : S1; // S1: Receive the audio
            S2: nextstate = transmit_ready ? S3 : S2; // S2: Process the audio
            S3: nextstate =             ss ? S4 : S3; // S3: Wait for master to be ready
            S4: nextstate =  transmit_done ? S0 : S4; // S4: Transmit the audio
            default: nextstate = S0;
        endcase
    end

    // Output logic
 //   always_comb begin
        //input_ready =    (count == 16'h9C40);  // Stop when we have received 5000 1-byte samples
   //     input_ready    = (count = 4'b1000);    // TODO: Shortened counter for testing
   //     transmit_ready = (data != 7'b0000000); // Ready to transmit when there is data to send
    //    enable_processing = (state == S2);      // Only process in state 2
 //   end

    spi_receive	receiver(state, sck, sdi, ss, audio, led, input_ready);
    spi_send		transmitter(state, sck, data, sdo, transmit_done);
    process_audio	processor(state, audio, data, transmit_ready);

endmodule


module spi_receive(input logic [2:0]  state,
						 input  logic       sck, sdi, en,
                   output logic [7:0] audio,
                   output logic [7:0] led,
                   output logic       input_ready);

logic [3:0] count;

    // Only receive if enable is high
    // TODO: Reset count at some point
    always_ff @(posedge sck)	
	     if ( (state == 3'b001) && en) begin
			  if (count == 4'b1000) begin
					count <= 4'b0;
					input_ready <= 1'b1;
					audio <= {audio[6:0], sdi};
			  end else begin
					count <= count + 4'b1;
					input_ready <= 1'b0;
					audio <= {audio[6:0], sdi};
			  end
		  end


    // TODO: For debugging, write the received data to the LED bar
    assign led = audio;

endmodule



module spi_send(input logic [2:0]  state,
					 input  logic       sck,
                input  logic [7:0] data,
                output logic       sdo,
					 output logic		  transmit_done);

logic [3:0] count;
    
	 // Only transmit if enable is high
    always_ff @(negedge sck)
        if (state == 3'b100) begin   
            if (count == 4'b0111) begin
                count <= 4'b0;
					 transmit_done = 1'b1;
					 
            end else begin
                count <= count + 4'b0001;
					 transmit_done = 1'b0;
            end

            sdo <= data[count];
            
        end

endmodule


module process_audio(//input  logic [39999:0] audio,
							input logic [2:0]	state,
                     input  logic     [7:0] audio,
                     output logic     [7:0] data,
                     output logic           transmit_ready);
     
    // TODO: Audio processing algorithm will go here
    // TODO: For now, write the received char back to data
    always_comb begin
        if (state == 3'b010) begin
            data = audio;
            transmit_ready = 1;
        end else
            data = '0;
            transmit_ready = '1;
    end
endmodule