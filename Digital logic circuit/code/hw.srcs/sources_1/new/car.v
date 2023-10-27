`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/11/11 21:17:20
// Design Name: 
// Module Name: car
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module car(
    input  [2:0]cin,
    input  clk,
    output wire light1,
    output wire light2,
    output reg [2:0]leftled,
    output reg [2:0]rightled,
    output wire [6:0]num1,
    output wire [6:0]num2
    );
    wire divclk1;
    wire divclk2;
    wire divclk3;
    reg [1:0]cnt1;
    wire cnt2;
    wire [1:0]cnt3;
    reg [2:0]timi;
    reg reset;
    assign light1 = 1;
    assign light2 = 1;
    //初始化
    initial begin
        leftled = 3'b000;
        rightled = 3'b000;
    end  
    //时钟分频
    timediv u1(clk, divclk1, divclk2, divclk3, divclk);
    count u2(divclk1,divclk2,cnt2,cnt3);
    digital u3(cin,1,num1,num2);
    //计时功能
    always@(negedge divclk)
        begin
            if(cin == 3'b001|| cin == 3'b111)
                begin
                    if(timi == 3'd7)
                        timi = 3'd7;
                    else 
                        timi = timi + 3'd1;
                end
            else
                timi = 3'd0;
        end
    always@(negedge divclk1)
        begin
            if(cin == 3'b010|| cin == 3'b011 || cin == 3'b101)
            begin
                if(cnt1 == 2'b11 || cnt1 == 2'b00 )
                begin
                cnt1 <= 2'b01;
                end
                else
                begin
                cnt1 <= cnt1 + 2'b1;
                end
            end
            else cnt1 = 2'b01;
        end
    //车辆尾灯控制
    always@(negedge divclk3)
        begin
            case(cin)
                3'b000://正向行驶
                    begin
                        leftled = 3'b000;
                        rightled = 3'b000;
                    end
                3'b001://车门未关紧
                    begin
                        if(timi < 3'd3)
                            begin
                                leftled = 3'b111;
                                rightled = 3'b111;
                            end
                        else
                            begin
                                leftled = 3'b000;
                                rightled = 3'b000;
                            end    
                    end
                3'b010://汽车右转弯
                    begin
                        leftled = 3'b000;
                        case(cnt1)
                            2'b01:rightled = 3'b001;
                            2'b10:rightled = 3'b010;
                            2'b11:rightled = 3'b100;
                            default:rightled = 3'b000;
                        endcase         
                    end
                3'b011://汽车左转弯
                    begin
                        rightled = 3'b000;
                        case(cnt1)
                            2'b01:leftled = 3'b001;
                            2'b10:leftled = 3'b010;
                            2'b11:leftled = 3'b100;
                            default:leftled = 3'b000;
                        endcase  
                    end
                3'b100://临时刹车
                    begin
                        case(cnt2)
                            1'b0:
                                begin
                                    leftled = 3'b000;
                                    rightled = 3'b000;
                                end
                            1'b1:
                                begin
                                    leftled = 3'b111;
                                    rightled = 3'b111;
                                end
                        endcase                            
                    end
                3'b101://故障状态
                    begin
                        case(cnt1)
                            2'b01:
                                begin
                                    rightled = 3'b001;
                                    leftled = 3'b001;
                                end
                            2'b10:
                                begin
                                    rightled = 3'b010;
                                    leftled = 3'b010;
                                end
                            2'b11:
                                begin
                                    rightled = 3'b100;
                                    leftled = 3'b100;
                                end
                            default:
                                begin
                                    rightled = 3'b000;
                                    leftled = 3'b000;
                                end
                        endcase  
                    end
                3'b110://倒车状态
                    begin
                        leftled = 3'b101;
                        rightled = 3'b101;
                    end
                3'b111://驻车状态
                    begin
                        if(timi < 3'd3)
                            begin
                                case(cnt3)
                                    2'b01:
                                        begin
                                            leftled = 3'b101;
                                            rightled = 3'b101;
                                        end
                                    2'b11:
                                        begin
                                            leftled = 3'b010;
                                            rightled = 3'b010;
                                        end
                                    default:
                                        begin
                                            leftled = 3'b000;
                                            rightled = 3'b000;
                                        end
                                endcase
                            end
                        else
                            begin
                                leftled = 3'b000;
                                rightled = 3'b000;
                            end 
                    end
            endcase
        end                                 
endmodule

//时钟分频
module timediv( input clk,
                output reg divclk1,
                output reg divclk2,
                output reg divclk3,
                output reg divclk
                );
    reg [31:0]counter1 = 32'd0;
    reg [31:0]counter2 = 32'd0;
    reg [31:0]counter3 = 32'd0;
    reg [31:0]counter4 = 32'd0;
    initial begin
        divclk1 = 0;
        divclk2 = 0;
        divclk3 = 0;
        divclk = 0;
    end                       
    always@(negedge clk)
        begin
            if(counter1 == 32'd1200_0000)
                begin
                    counter1 <= 32'd0;
                    divclk1 <= ~divclk1;
                end
            else
                counter1 <= counter1 + 32'd1;
        end
    always@(negedge clk)
        begin 
            if(counter2 == 32'd600_0000)
                begin
                    counter2 <= 32'd0;
                    divclk2 <= ~divclk2;
                end
            else
                counter2 <= counter2 + 32'd1;
        end
    always@(negedge clk)
        begin       
            if(counter3 == 32'd100_0000)
                begin
                    counter3 <= 32'd0;
                    divclk3 <= ~divclk3;
                end
            else
                counter3 <= counter3 + 32'd1;
        end
    always@(negedge clk)
        begin        
            if(counter4 == 32'd1_0000_0000)
                begin
                    counter4 <= 32'd0;
                    divclk <= ~divclk;
                end
            else
                counter4 <= counter4 + 32'd1;
        end                     
endmodule

//计数器
module count(
              input divclk1,
              input divclk2,
              output reg cnt2,
              output reg [1:0]cnt3
              );
    initial begin
        cnt2 = 1'b0;
        cnt3 = 2'b00;
    end
    always@(negedge divclk2)
        begin      
            if(cnt2 == 1'b1)
                cnt2 <= 1'b0;
            else
                cnt2 <= cnt2 + 1'b1;
        end
    always@(negedge divclk1)
        begin        
            if(cnt3 == 2'b11)
                begin
                cnt3 <= 2'b00;
                end
            else
                begin
                    cnt3 <= cnt3 + 2'd1;
                end
        end    
endmodule

module digital(
        input [2:0] a,
		input  en,
        output reg [6:0] num1,
        output reg [6:0] num2
    );

initial
    begin
    num1=7'b0000_000;
    num2=7'b0000_000;
    end 
	
	
always @(a)
    begin
    if(en==0) 
     begin
     num1=7'b0000_000;
     num2=7'b0000_000;
     end
     
     else
      begin
        case(a)
                 3'b000: 
                     begin
                         num1=7'b0000_110;
                         num2=7'b0000_110;
                     end 
                
                 3'b001: 
                      begin
                         num2=7'b1111_001;
                          num1=7'b1111_001;
                      end
               
                 3'b010: 
                      begin
                           num2=7'b0000_110;
                           num1=7'b0111_111;
                      end
                
                 3'b011: 
                      begin
                         num2=7'b0111_111;
                         num1=7'b0000_110;
                      end
                
                 3'b100: 
                      begin
                         num2=7'b1110_011;
                         num1=7'b1110_011;
                      end
               
                 3'b101: 
                      begin
                         num1=7'b0111_111;
                         num2=7'b0111_111;
                      end
                 
                 3'b110: 
                      begin
                        num1=7'b1110_001;
                        num2=7'b1110_001;
                      end
                     
                 3'b111:
                      begin
                          num1=7'b0111_000;
                          num2=7'b0111_000;
                       end
                 
     endcase
     end
     
    end
    
endmodule

