// Copyright (C) 1991-2013 Altera Corporation
// Your use of Altera Corporation's design tools, logic functions 
// and other software and tools, and its AMPP partner logic 
// functions, and any output files from any of the foregoing 
// (including device programming or simulation files), and any 
// associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License 
// Subscription Agreement, Altera MegaCore Function License 
// Agreement, or other applicable license agreement, including, 
// without limitation, that your use is for the sole purpose of 
// programming logic devices manufactured by Altera and sold by 
// Altera or its authorized distributors.  Please refer to the 
// applicable agreement for further details.

// PROGRAM		"Quartus II 64-Bit"
// VERSION		"Version 13.0.1 Build 232 06/12/2013 Service Pack 1 SJ Web Edition"
// CREATED		"Thu Nov 28 14:35:56 2024"

module Lab01(
	SW_0,
	SW_1,
	SW_2,
	SW_3,
	SW_4,
	SW_5,
	SW_6,
	SW_7,
	Button_0,
	Button_1,
	Button_2,
	SEG7_0_D0,
	SEG7_0_D1,
	SEG7_0_D2,
	SEG7_0_D3,
	SEG7_0_D4,
	SEG7_0_D5,
	SEG7_0_D6,
	SEG7_0_DP,
	LED0,
	LED1,
	LED2
);


input wire	SW_0;
input wire	SW_1;
input wire	SW_2;
input wire	SW_3;
input wire	SW_4;
input wire	SW_5;
input wire	SW_6;
input wire	SW_7;
input wire	Button_0;
input wire	Button_1;
input wire	Button_2;
output wire	SEG7_0_D0;
output wire	SEG7_0_D1;
output wire	SEG7_0_D2;
output wire	SEG7_0_D3;
output wire	SEG7_0_D4;
output wire	SEG7_0_D5;
output wire	SEG7_0_D6;
output wire	SEG7_0_DP;
output wire	LED0;
output wire	LED1;
output wire	LED2;





assign	SEG7_0_D0 =  ~SW_0;

assign	SEG7_0_D1 =  ~SW_1;

assign	LED2 =  ~Button_2;

assign	SEG7_0_D2 =  ~SW_2;

assign	SEG7_0_D3 =  ~SW_3;

assign	SEG7_0_D4 =  ~SW_4;

assign	SEG7_0_D5 =  ~SW_5;

assign	SEG7_0_D6 =  ~SW_6;

assign	SEG7_0_DP =  ~SW_7;

assign	LED0 =  ~Button_0;

assign	LED1 =  ~Button_1;


endmodule
