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

 module peripherals(input  logic clk, sck, sdi, reset,      // from master
                    input  logic [2:0] result,              // result of processing
                    output logic sdo,
                    output logic [3:0] bar_R, bar_G,        // bicolor bargraph
                    output logic       led_rdy, led_rec, led_R, led_G, led_Y, led_O,
						  output logic [3:0] led);

  // Extra signals
  logic [7:0] data_receive, junk_send;  // for SPI communication
  logic [2:0] sys_state;       // current state of the system, result of the PIC slave
  
  // Debugging signals
  assign led = sys_state;
  
  logic  ready_to_listen, listening, processing, done, restart, fail;

  // State signals
  typedef enum logic [1:0] {GET_STATE=2'b01, GET_RESULT=2'b10} statetype;
  statetype state, nextstate;

  // State register
  always_ff @(posedge clk) begin
    if (reset)
	     state <= GET_STATE;
    else
        state <= nextstate;
  end

  // Next state logic
  always_comb begin
    case(state)
       GET_STATE: nextstate =    done ? GET_RESULT : GET_STATE;
      GET_RESULT: nextstate = restart ? GET_STATE  : GET_RESULT;
         default: nextstate = GET_STATE;
    endcase
  end
  
  always_comb begin
    // System state/result logic
	 if (reset)
	   sys_state = 3'b001;
    else
      sys_state = (state == GET_STATE)  ? data_receive[2:0] : 3'b001;

    // Logic assigned to each state of the system
    ready_to_listen = (sys_state == 3'b001);
    listening       = (sys_state == 3'b010);
    processing      = (sys_state == 3'b011);
    done            = (sys_state == 3'b100);
	 restart         = (led_R | led_G | led_Y | led_O);

    // Ready light is on if ready to listen
    led_rdy = ready_to_listen;

    // Recording light is on if currently listening
    led_rec = listening;

    // Color LED is lit if it matches the result of the processing
    led_R = (ready_to_listen | done) ? (result == 3'b001) : 1'b1;
    led_G = (ready_to_listen | done) ? (result == 3'b010) : 1'b1;
    led_Y = (ready_to_listen | done) ? (result == 3'b011) : 1'b1;
    led_O = (ready_to_listen | done) ? (result == 3'b100) : 1'b1;
	 
	 fail = (result == 3'b000);
  end
                         
  
  spi_send_receive spi (state, reset, sck, sdi, sdo, junk_send, data_receive, data_exchanged);
  display_bargraph displayer (clk, reset, ready_to_listen, listening, processing, done, fail, bar_R, bar_G);

endmodule

module display_bargraph(input  logic clk, reset,
                        input  logic ready_to_listen, listening, processing, done, fail,
                        output logic [3:0] bar_R, bar_G);
  
  logic [23:0] led_clk;
  logic  [2:0] cnt; 
  
	always_comb begin		
		// Processing: cycle through the red LEDs hypnotizer style
		if (processing) begin
			bar_G = 4'b0000;

			case(cnt)
				3'b000: bar_R = 4'b0001;
				3'b001: bar_R = 4'b0010;
				3'b010: bar_R = 4'b0100;
				3'b011: bar_R = 4'b1000;
				3'b100: bar_R = 4'b0100;
				3'b101: bar_R = 4'b0010;
			  default: bar_R = 4'b0001;
			endcase
		end
		
		else if (ready_to_listen) begin
			bar_G[3] = ~fail;
			bar_G[2] = ~fail;
			bar_G[1] = ~fail;
			bar_G[0] = ~fail;

			bar_R[3] = fail;
			bar_R[2] = fail;
			bar_R[1] = fail;
			bar_R[0] = fail;   
		end
		
		else begin
			// If done display all green
			bar_G[3] = (reset | ready_to_listen | done);
			bar_G[2] = (reset | ready_to_listen | done);
			bar_G[1] = (reset | ready_to_listen | done);
			bar_G[0] = (reset | ready_to_listen | done);

			bar_R[3] = (~ready_to_listen & ~processing & ~done);
			bar_R[2] = (~ready_to_listen & ~processing & ~done);
			bar_R[1] = (~ready_to_listen & ~processing & ~done);
			bar_R[0] = (~ready_to_listen & ~processing & ~done);
		end
	end
	
	always_ff @(posedge clk) begin
		if (reset || listening || ready_to_listen || done) begin 
			led_clk <= 23'b0;
			cnt <= 3'b0;
		end else begin
			led_clk <= led_clk + 23'b1;
			
			if (cnt > 3'b101) begin
				cnt <= 3'b000;
			end
			else if (led_clk[22]) begin
				cnt <= cnt + 3'b1;
			end
		end
	
	end
endmodule

module spi_send_receive(input  logic       state, reset,
                        input  logic       sck, sdi,
                        output logic       sdo,
                        input  logic [7:0] d,   // data to send
                        output logic [7:0] q,   // data received
                        output logic       data_exchanged);
                         
  logic [2:0] cnt;
  logic [7:0] buffer;
  logic qdelayed;

  always_ff @(negedge sck) begin
     if (reset) begin
	      cnt <= 0;
			q <= 0;
	  end
	  else
    	  cnt <= cnt + 3'b1;
     
     if (cnt >= 3'd7) begin
         q <= buffer;
			data_exchanged <= (state == 2'b10);
     end
  end
    
  always_ff @(posedge sck) begin
    if (reset)
	     buffer <= 7'b0;
	 else
        buffer <= (cnt == 0) ? d : {buffer[6:0], sdi}; //shift register
  end

  always_ff @(negedge sck)
    qdelayed <= buffer[7];

  assign sdo = (cnt == 0) ? d[7] : qdelayed;
endmodule
