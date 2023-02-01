module traffic_light (
    input  clk,
    input  rst,
    input  pass,
    output reg R,
    output reg G,
    output reg Y
);
	reg [9:0] cycle;
	reg [2:0] state;
	reg state_count;
	parameter initial_state = 3'd0;
	parameter nosignal = 3'd1;
	parameter green = 3'd2;
	parameter yellow = 3'd3;
	parameter red = 3'd4;
	initial
		begin
			cycle <= 10'd0;
			state <= initial_state;
			state_count <= 1'b0;
			R<=1'b0;
			G<=1'b0;
			Y<=1'b0;			
		end
	always@(posedge clk)
		begin
			if(pass == 1'b1)
				begin
					if(state != initial_state)
						begin
							cycle = 10'd0;
							state = initial_state;
							state_count = 1'b0;
						end	
					else
						begin
							cycle = cycle + 10'd1;
						end
				end
			else
				begin
					if(rst == 1'b0)
						begin
							cycle = cycle + 10'd1;
						end
					if(cycle == 10'd512 && state == initial_state)
						begin
							cycle = 10'd0;
							state = nosignal;
						end
					else if(cycle == 10'd64 && state == nosignal)
						begin
							cycle = 10'd0;
							state = green;
						end
					else if(cycle == 10'd64 && state == green)
						begin
							cycle = 10'd0;
							if(state_count == 1'b0)
								begin
									state = nosignal;
								end
							else
								begin
									state = yellow;
								end
							state_count = state_count + 1'b1;
						end
					else if(cycle == 10'd256 && state == yellow)
						begin
							cycle = 10'd0;
							state = red;
						end
					else if(cycle == 10'd512 && state == red)
						begin	
							cycle = 10'd0;
							state = initial_state;
						end
				end						
		end
	always@(posedge clk)
	  begin
	   case(state)
	       green,initial_state:
	          begin
	           Y=1'b0;
	           G=1'b1;
	           R=1'b0;
	          end
	       red:
	          begin
	           Y=1'b0;
	           G=1'b0;
	           R=1'b1;
	          end
	       yellow:
	          begin
	           Y=1'b1;
	           G=1'b0;
	           R=1'b0;
	          end
	       default:
	          begin
	            Y=1'b0;
	            G=1'b0;
	            R=1'b0;
	          end
	     endcase
	  end
	always@(posedge rst)
		begin
			cycle <= 10'd0;
			state <= initial_state;
			state_count <= 1'b0;
		end
endmodule
