`timescale 1ns/10ps
module CS(Y,X, reset, clk);
	input clk, reset;
	input [7:0] X; 
	output reg	[9:0] Y;
  	reg unsigned [3:0] count;
	reg unsigned [11:0] sum;
  	reg unsigned [7:0] array [8:0];  
	reg unsigned [3:0] i;
	reg unsigned [8:0] min;
	reg unsigned [3:0] min_index;
	reg first;
	initial
   		begin
			count <= 4'b0000;
	    		sum <= 12'b000000000;
			array[0] <= 8'b00000000;
			array[1] <= 8'b00000000;
			array[2] <= 8'b00000000;
			array[3] <= 8'b00000000;
			array[4] <= 8'b00000000;
			array[5] <= 8'b00000000;
			array[6] <= 8'b00000000;
			array[7] <= 8'b00000000;
			array[8] <= 8'b00000000;
	      		min <= 9'b000000000;
	       		min_index <= 4'b0000;
	       		first <= 1'b1;
			Y <= 10'b0000000000;
			i<=4'd0;
	   	end
	
	always@(posedge clk or posedge reset)
		 begin 
			if(reset)
				begin
					sum <= 12'b000000000;
					count <= 4'b0000;
					Y <= 10'b0000000000;
	       				min_index <= 4'b0000;
					first <= 1'b1;
				end
			else
				begin
          				array[count] <= X;
          				sum <= sum + X;
          				count <= count + 4'b0001;
        			end
		end
	always@(count)
		begin
			if(count == 4'b1001)
              			begin
                  			count <= 4'b0000;
                  			if(first == 1'b1)
                     				begin
                        				first <= 1'b0;
                      				end
				end
			if(first == 1'b0)
				begin	
					min = 9'b011111111;
					min_index = 4'b0000;
                 			for(i = 4'b0000;i < 4'b1001;i=i+4'b0001)
					begin
                       	  			if((sum / 4'b1001) >= array[i])                   
                              			begin
                                			if(min > (sum / 4'b1001 - array[i]))      
						        begin
                                  		  		min = (sum / 4'b1001 - array[i]); 
                                    				min_index = i;                    
						        end
                     	        		end
         	            		end 
         	        		Y = #0.6 (sum + 4'b1001 * array[min_index])/ 4'b1000;     
					min_index = 4'b0000;	
         	        		sum = sum - array[count];   
				end
		end
endmodule



