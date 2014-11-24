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
    logic input_ready;     // Keep track of whether or not input audio has been
                           // received and processing is ready to begin
    logic transmit_ready;  // Keep track of whether or not processing is complete
                           // and data is ready to be sent back to master
    logic [7:0]     data_send, data_receive, junk_send, junk_receive;
    byte            audio[1999:0]; // TODO: Received data for testing
    logic [3:0]     count; // TODO: Shortened counter for testing
    logic         enable_processing;
    logic           complete_send;
    logic            transmit_done;
    

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
    
    spi_send_receive receiver(state, sck, sdo, sdi, reset, junk_send, data_receive, input_ready);
    
     // FIXME vvv
     store store_int(state, sck, data_receive, input_ready, audio);
     // FIXME ^^^
     
    spi_send_receive transmitter(state, sck, sdo, sdi, reset, data_send, junk_receive, transmit_done);
    compareFromBank compareFromBank_inst(state, clk, audio, result, transmit_ready);

endmodule


module spi_send_receive(input logic[2:0] state,
                        input logic sck, // from master 
                        input logic sdo, // from master
                        output logic sdi, // to master
                        input logic reset,
                        input logic [7:0] d, // data to send 
                        output logic [7:0] q, // data received
                        output logic flag); // dependent on state

    logic [2:0] cnt = 0; 
    logic qdelayed;

    // 3-bit counter tracks when full byte is transmitted and new d should be sent
    always_ff @(negedge sck, posedge reset)

        if (reset) begin
            cnt <= 0;
            flag <= 0;
        end
        else if (cnt == 3'b111)  begin
            cnt <= cnt + 3'b1;

            // Set flag after all data is exchanged
            flag <= (1 & (state == 3'b001 || state == 3'b100)); 
        end
        else
            cnt <= cnt + 3'b1;


    // loadable shift register
    // loads d at the start, shifts sdo into bottom position on subsequent step 
    always_ff @(posedge sck)
        q <= (cnt == 0) ? d : {q[6:0], sdo};

    // align sdi to falling edge of sck // load d at the start
    always_ff @(negedge sck)
        qdelayed = q[7];

    assign sdi = (cnt == 0) ? d[7] : qdelayed;

endmodule
