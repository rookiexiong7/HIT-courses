`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/11/26 20:58:30
// Design Name: 
// Module Name: sim_2
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


module sim_2();
reg divclk1,divclk2;
wire [1:0]cnt1;wire cnt2; wire [1:0]cnt3;
initial begin
    divclk1 = 0;
    divclk2 = 0;
end
always #60 divclk1 = ~divclk1;
always #30 divclk2 = ~divclk2;

count u22(divclk1,divclk2,cnt1,cnt2,cnt3);


endmodule
