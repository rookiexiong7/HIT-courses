`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/11/26 20:48:09
// Design Name: 
// Module Name: sim_1
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


module sim_1(
    );
    wire divclk1,divclk2,divclk3,divclk;
    reg clk;
    initial begin
    clk = 0;
    end
    always #1 clk=~clk;
    timediv u11(clk,divclk1,divclk2,divclk3,divclk);
    
endmodule
