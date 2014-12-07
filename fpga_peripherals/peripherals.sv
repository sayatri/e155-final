/* peripherals.sv
 *
 * strisorus@hmc.edu
 * 06 December 2014
 *
 * This code:
 *    - Manages peripheral output for Speech Recognition System
 *    - Gets state information from a master PIC via SPI and displays relevant
 *        information according to the state
 *    - Gets the result of signal processing and visually displays the resukt
 *        on LEDs
 */

 module peripherals(input  logic clk, sck, sdi, ss, reset,    // from master
                    output logic sdo,
                    output logic [3:0] bar_R, bar_G,        // bicolor bargraph
                    output logic led_rdy, led_rec, led_R, led_G, led_Y, led_O,
						  output [7:0] sys_state);

  // Extra signals
  logic [7:0] data_receive, junk_send;  // for SPI communication
  logic [7:0] result;                   // result of the PIC slave
  logic       data_exchanged;

  //logic [7:0] sys_state;                // current state of the system
  logic       ready_to_listen, listening, processing, done;

  // State signals
  typedef enum logic [1:0] {GET_STATE=2'b01, GET_RESULT=2'b10} statetype;
  statetype state, nextstate;

  // State register
  always_ff @(posedge clk)
    state <= nextstate;

  // Next state logic
  always_comb begin
    case(state)
              GET_STATE: nextstate =           done ? GET_RESULT : GET_STATE;
             GET_RESULT: nextstate = data_exchanged ? GET_STATE  : GET_RESULT;
                default: nextstate = GET_STATE;
    endcase
  end


  always_comb begin
    // System state/result logic
	 if (reset) begin
	   sys_state = 8'b0;
		result = 8'b0;
	 end
    else if (state == GET_STATE) begin
      sys_state = data_receive;
      result = 8'b0;
    end
    else begin
      sys_state = 8'd10;
      result = data_receive;
    end

    // Logic assigned to each state of the system
    ready_to_listen = (sys_state == 8'd1);
    listening       = (sys_state == 8'd2);
    processing      = ((sys_state == 8'd3) | 
								(sys_state == 8'd4) | 
									(sys_state == 8'd5) | 
									(sys_state == 8'd6) | 
									(sys_state == 8'd7) |
									(sys_state == 8'd8) | 
									(sys_state == 8'd9));
    done            = (sys_state == 8'd10);

    // Ready light is on if ready to listen
    led_rdy = ready_to_listen;

    // Recording light is on if currently listening
    led_rec = listening;

    // Color LED is lit if it matches the result of the processing
    led_R = (done & (result == 8'b001));
    led_G = (done & (result == 8'b010));
    led_Y = (done & (result == 8'b011));
    led_O = (done & (result == 8'b101));
  end
                         
  
  spi_send_receive spi (state, sck, sdi, sdo, junk_send, data_receive, data_exchanged);
  display_bargraph displayer (clk, reset, ready_to_listen, listening, processing, done, bar_R, bar_G);

endmodule

module display_bargraph(input  logic clk, reset,
                        input  logic ready_to_listen, listening, processing, done,
                        output logic [3:0] bar_R, bar_G);
  
  logic [22:0] led_clk;
  logic  [2:0] cnt; 
  
  
  
	always_comb begin
	
		// If ready to listen or listening display all red
		if (reset || ready_to_listen || listening) begin
			bar_R = 4'b1111;
			bar_G = 4'b0000;
			led_clk = 23'b0;
			
		// If done display all green
		end else if (done) begin
			bar_R = 4'b0000;
			bar_G = 4'b1111;
			led_clk = 23'b0;
			
		// Otherwise cycle through the red LEDs hypnotizer style
		end else begin
			led_clk = led_clk + 23'b1;
			bar_G = 4'b0000;
			case(cnt)
				3'b000: bar_R <= 4'b0001;
				3'b001: bar_R <= 4'b0010;
				3'b010: bar_R <= 4'b0100;
				3'b011: bar_R <= 4'b1000;
				3'b100: bar_R <= 4'b0100;
				3'b101: bar_R <= 4'b0010;
			  default: bar_R <= 4'b0000;
			endcase
		end
	end
	
	always_ff @(posedge clk) begin
		if (reset) begin 
			led_clk <= 23'b0;
			cnt <= 3'b0;
		end else begin
			led_clk <= led_clk + 23'b1;
			
			if (cnt > 3'b101) begin
				cnt <= 3'b000;
			end else begin
				cnt <= cnt + 3'b1;
			end
		end
	
	end
endmodule

module spi_send_receive(input  logic       state,
                        input  logic       sck, sdi,
                        output logic       sdo,
                        input  logic [7:0] d,   // data to send
                        output logic [7:0] q,   // data received
                        output logic       data_exchanged);
                         
  logic [2:0] cnt;
  logic [7:0] buffer;
  logic qdelayed;
  
  always_comb begin
    data_exchanged = ((cnt >= 3'd7) & (state == 2'b10));
  end

  always_ff @(negedge sck) begin
    cnt <= cnt + 3'b1;
     
     if (cnt >= 3'd7) begin
         q <= buffer;
     end
  end
    
  always_ff @(posedge sck)
    buffer <= (cnt == 0) ? d : {buffer[6:0], sdi}; //shift register

  always_ff @(negedge sck)
    qdelayed <= buffer[7];

  assign sdo = (cnt == 0) ? d[7] : qdelayed;
endmodule
