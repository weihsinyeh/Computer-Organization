module SME(clk,reset,chardata,isstring,ispattern,valid,match,match_index);
	input clk;
	input reset;
	input [7:0] chardata;
	input isstring;
	input ispattern;
	output match;
	output [4:0] match_index;
	output valid;
	reg match;
	reg [4:0] match_index;
	reg [4:0] first;
	reg valid;
	reg [7:0] sc [7:0];
	reg [7:0] ss [31:0];
	reg [5:0] i;
	reg [3:0] j;
	reg [3:0] find;
	reg [4:0] pi;
	reg [5:0] si;
	reg [5:0] tmp;
	reg ft,compare_head,compare_normal;

	initial
	begin
		first <= 5'b00000;
		i <= 6'b000000;
		j <= 4'b0000;
		find <= 4'b0000;
		match <= 1'b0;
		match_index <= 5'b00000;
		valid <= 1'b0;
		pi <= 5'b00000;
		si <= 6'b000000;
		tmp <= 6'b000000;
		ft <= 1'b0;
		compare_head <=1'b0;
		compare_normal <= 1'b0;
	end

	always@(posedge clk)                        //sequential circuit
	begin
		first = 5'b00000;
		match_index = 5'b00000;
		valid = 1'b0;
		match = 1'b0;
		compare_head =1'b0;
		compare_normal = 1'b0;
		if(reset == 1'b1)
		begin
			first = 5'b00000;
			i = 6'b000000;
			j = 4'b0000;
			find = 4'b0000;
			match = 5'b00000;
			match_index = 5'b00000;
			valid = 1'b0;
			pi = 5'b00000;
			compare_head =1'b0;
			compare_normal = 1'b0;
		end

		else
		begin

			if(ispattern == 1'b1) //讀輸入patter
			begin
				sc[pi] = chardata;
				pi = pi + 5'b00001;
				if(ft == 1'b1)
				begin
					si = tmp;
					tmp = 6'b000000;
					ft = 1'b0;
				end
			end

			else if(isstring == 1'b1) //讀輸入string
			begin
				ss[tmp] = chardata;
				tmp = tmp + 6'b000001;
				if(ft == 1'b0)
				begin
					ft = 1'b1;
					pi = 5'b00000;
				end
			end

			else if(pi!=5'd0 && si!= 6'd0) //輸入完pattern 與 string
			begin
				if(sc[0] == "^")
					compare_head = 1'b1;
				else
					compare_normal = 1'b1;
			end
		end
	end

	always@(posedge compare_head)                                               //combinational circuit
	begin
		for(i = 6'b000000;i < si;i=i+6'b000001) //0-31 最32個bit
		begin
					
			//如果第一個字一樣 或 第一個字可為任意字元 才繼續比下一個 
			if((sc[4'b0001] == ss[i] || sc[4'b0001] == ".") && (i== 6'b000000 ||ss[i-4'b0001] == " ")) 
			begin
				if(find == 4'b0000)
				begin
					first = i;               //紀錄起始位址
					find = find + 4'b0001;   //比對相同多一個find就加一
				end
				for(j = 4'b0001;j < pi;j=j+4'b0001)
				begin
					if(sc[j] == ss[i+j-4'b0001] || sc[j] == ".")             //如果相同
					begin
						find = find + 4'b0001;
						//字串相符
						if(find == pi)     
						begin
							match = 1'b1;
							match_index = first;
							j =  4'b0111;      //讓第二層迴圈停止
							i = 6'b100000;     //讓第一層迴圈停止
						end
					end
					else                               //如果不相同
					begin
						if(sc[j] == "$" && (ss[i+j-4'b0001] == " " || i+j-4'b0001 == si))
						begin
							match = 1'b1;
							match_index = first;
							j =  4'b0111;      //讓第二層迴圈停止
							i = 6'b100000;     //讓第一層迴圈停止
						end
						else
						begin
							j =  4'b0111;      //讓第二層迴圈停止
							i = first;         //讓i回到原本的值
						end
					end
				end
			end
			find = 4'b0000;
		end
		valid = 1'b1;	
		compare_head = 1'b0;
		pi = 5'b00000;
	end

	always@(posedge compare_normal)                      //combinational circuit
	begin          
		
		for(i = 6'b000000;i < si;i=i+6'b000001) //0-31
		begin
			//如果第一個字元一樣才繼續比下一個 或是 第一個字元是任意字元
			if(sc[0] == ss[i] || sc[0] == ".") 
			begin
				if(find == 4'b0000)
					first = i; //紀錄起始位址	
					
				for(j = 4'b0000;j < pi;j=j+4'b0001)
				begin
					if(sc[j] == ss[i+j] || sc[j] == ".")          //如果相同
					begin
						find = find + 4'b0001;
						if(find == pi)     //字串相符
						begin
							match = 1'b1;
							match_index = first;
							j =  4'b0111; //讓第二層迴圈停止
							i = 6'b100000;//讓第一層迴圈停止
						end
						if(find == pi-5'b00001 && (sc[j+4'b0001] == "$" && i+j+4'b0001 == si))
						begin
							match = 1'b1;
							match_index = first;
							j =  4'b0111; //讓第二層迴圈停止
							i = 6'b100000;//讓第一層迴圈停止
						end
					end
					else                             //如果不相同
					begin
						if(sc[j] == "$" && ss[i+j] == " ")
						begin
							match = 1'b1;
							match_index = first;
							j =  4'b0111; //讓第二層迴圈停止
							i = 6'b100000;//讓第一層迴圈停止
						end
						else
						begin
							j =  4'b0111; //讓第二層迴圈停止
							i = first;    //讓i回到原本的值
						end
					end
				end
			end
			find = 4'b0000;	
		end
		valid = 1'b1;
		pi = 5'b00000;
		compare_normal = 1'b0;
	end
endmodule
