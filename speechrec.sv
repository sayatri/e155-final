// speechrec.sv
// 16 November 11
// strisorus@hmc.edu
// 
// Receives an audio signal via SPI and compares it to audio signals stored in memory

module speechrec(input  logic       clk, sck, sdi, //ss,
                 output logic       sdo,
                 output logic [7:0] led,
					  output logic [3:0] rec_clk);

 
    //logic [15:0]    count; // Keep count of bits of data received
    logic [3:0]     count; // TODO: Shortened counter for testing
	 logic [7:0] data_copy;
	 logic 		enable_processing;
	 logic 		complete_send;



    spi_receive	receiver(sck, sdi, audio, led, rec_clk);
  //  spi_send		transmitter(sck, input_ready, data, sdo);

endmodule


module spi_receive(
						 input  logic       sck, sdi,
                   output logic [7:0] audio,
                   output logic [7:0] led,
                   //output logic       input_ready,
						 output logic [3:0] rec_clk);

logic [3:0] count;
logic [7:0] buffer;

    // Only receive if enable is high
    // TODO: Reset count at some point
    always_ff @(posedge sck)	begin
           buffer <= {buffer[6:0], sdi};
			  count <= count + 4'b1;

			  if (count >= 4'b1000) begin
					audio <= buffer;
					count <= 4'b0;
					//input_ready <= 1'b1;
			  end else begin
					//input_ready <= 1'b0;
			  end
	 end

    // TODO: For debugging, write the received data to the LED bar
    assign led = audio;
	 assign rec_clk = count;

endmodule

//
//
//module spi_send(
//					 input  logic       sck,
//					 input logic 		  input_ready,
//                input  logic [7:0] data,
//                output logic       sdo);
//
//logic [3:0] count;
//    
//	 // Only transmit if enable is high
//    always_ff @(negedge sck)
//        if (state == 3'b100) begin   
//            if (count == 4'b0111) begin
//                count <= 4'b0;
//					 transmit_done = 1'b1;
//					 
//            end else begin
//                count <= count + 4'b0001;
//					 transmit_done = 1'b0;
//            end
//
//            sdo <= data[count];
//            
//        end

//endmodule
