`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/11/26 21:07:23
// Design Name: 
// Module Name: sim_3
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


module sim_3();
reg [2:0]cin;
reg clk;
wire light1;
wire light2;
wire [2:0]leftled;
wire [2:0]rightled;
wire [6:0]num1;
wire [6:0]num2;
initial begin
    cin = 3'd0;
    clk = 0;
end
always #1 clk =~clk;
always #150 cin = cin + 3'd1;
car u33(cin,clk,light1,light2,leftled,rightled,num1,num2);

endmodule
