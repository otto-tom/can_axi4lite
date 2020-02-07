/*  ******************************************************************************\
 * /------------------------------------------------------------------------------/
 * |-- Title      : AXI CAN controller testbench
 * |-- Design Name: AXI4-lite CAN controller
 * |#----------------------------------------------------------------------------#
 * |-- File       : axi_can_tb.v
 * |-- Module Name: axi_can_tb
 * |-- Author     : Othon Tomoutzoglou  <otto_sta@hotmail.com>
 * |-- Company    : Hellenic Mediterranean University, department of
 * |--              Electrical & Computer Engineering, ISCA-lab
 * |-- URL        : http://isca.hmu.gr/
 * |-- Created    : 2018-10-22
 * |-- Last update: 2018-10-22
 * |-- License    :
 * |-- Platform   :
 * |-- Standard   :  IEEE Standard 1364-2001 / Verilog-2001
 * |#----------------------------------------------------------------------------#
 * |-- Description: 
 * |#----------------------------------------------------------------------------#
 * |-- Copyright (c) 2018
 * |#----------------------------------------------------------------------------#
 * |-- Revisions  :
 * |-- Date        Version  Author  Description
 * |-- 2018-10-22  1.0      Otto    Created
 * \#-----------------------------------------------------------------------------\
 * *******************************************************************************/

`timescale 1 ns / 1 ps

`include "can_defines.v"
`include "can_testbench_defines.v"

// Define Module for Test Fixture
module axi_can_tb();

    reg clk;
    reg rstn;     

    // CAN0 ports
    wire i_can0_bus_in;
    wire o_can0_bus_out;
    wire o_irq0;
    wire can0_clkout_o;
    // Ports of Axi Slave Bus Interface
    reg [7 : 0] s00_axi_awaddr;
    reg  s00_axi_awvalid;
    wire s00_axi_awready;
    reg [31 : 0] s00_axi_wdata;
    reg [3 : 0] s00_axi_wstrb;
    reg  s00_axi_wvalid;
    wire s00_axi_wready;
    wire [1 : 0] s00_axi_bresp;
    wire s00_axi_bvalid;
    reg  s00_axi_bready;
    reg [7 : 0] s00_axi_araddr;
    //AWPROT and ARPROT - required by Altera
    //wire [2 : 0] s00_axi_arprot;
    //wire [2 : 0] s00_axi_awprot;
    reg  s00_axi_arvalid;
    wire s00_axi_arready;
    wire [31 : 0] s00_axi_rdata;
    wire [1 : 0] s00_axi_rresp;
    wire s00_axi_rvalid;
    reg  s00_axi_rready;

    // CAN1 ports
    wire i_can1_bus_in;
    wire o_can1_bus_out;
    wire o_irq1;
    wire can1_clkout_o;
    // Ports of Axi Slave Bus Interface
    reg [7 : 0] s01_axi_awaddr;
    reg  s01_axi_awvalid;
    wire s01_axi_awready;
    reg [31 : 0] s01_axi_wdata;
    reg [3 : 0] s01_axi_wstrb;
    reg  s01_axi_wvalid;
    wire s01_axi_wready;
    wire [1 : 0] s01_axi_bresp;
    wire s01_axi_bvalid;
    reg  s01_axi_bready;
    reg [7 : 0] s01_axi_araddr;
    //AWPROT and ARPROT - required by Altera
    //wire [2 : 0] s01_axi_arprot;
    //wire [2 : 0] s01_axi_awprot;
    reg  s01_axi_arvalid;
    wire s01_axi_arready;
    wire [31 : 0] s01_axi_rdata;
    wire [1 : 0] s01_axi_rresp;
    wire s01_axi_rvalid;
    reg  s01_axi_rready;

    // Instantiate the CAN0
    axi_can CAN0 (
        .i_can_bus_in(i_can0_bus_in),
        .o_can_bus_out(o_can0_bus_out),
        .o_irq(o_irq0),
        .can_clkout_o(can0_clkout_o),
        // Ports of Axi Slave Bus Interface
        .s_axi_aclk(clk),
        .s_axi_aresetn(rstn),
        .s_axi_awaddr(s00_axi_awaddr),
        .s_axi_awvalid(s00_axi_awvalid),
        .s_axi_awready(s00_axi_awready),
        .s_axi_wdata(s00_axi_wdata),
        .s_axi_wstrb(s00_axi_wstrb),
        .s_axi_wvalid(s00_axi_wvalid),
        .s_axi_wready(s00_axi_wready),
        .s_axi_bresp(s00_axi_bresp),
        .s_axi_bvalid(s00_axi_bvalid),
        .s_axi_bready(s00_axi_bready),
        .s_axi_araddr(s00_axi_araddr),
        //AWPROT and ARPROT - required by Altera
        //input wire [2 : 0] s_axi_arprot,
        //input wire [2 : 0] s_axi_awprot,
        .s_axi_arvalid(s00_axi_arvalid),
        .s_axi_arready(s00_axi_arready),
        .s_axi_rdata(s00_axi_rdata),
        .s_axi_rresp(s00_axi_rresp),
        .s_axi_rvalid(s00_axi_rvalid),
        .s_axi_rready(s00_axi_rready)
        );

     // Instantiate the CAN1
     axi_can CAN1 (
        .i_can_bus_in(i_can1_bus_in),
        .o_can_bus_out(o_can1_bus_out),
        .o_irq(o_irq1),
        .can_clkout_o(can1_clkout_o),
        // Ports of Axi Slave Bus Interface
        .s_axi_aclk(clk),
        .s_axi_aresetn(rstn),
        .s_axi_awaddr(s01_axi_awaddr),
        .s_axi_awvalid(s01_axi_awvalid),
        .s_axi_awready(s01_axi_awready),
        .s_axi_wdata(s01_axi_wdata),
        .s_axi_wstrb(s01_axi_wstrb),
        .s_axi_wvalid(s01_axi_wvalid),
        .s_axi_wready(s01_axi_wready),
        .s_axi_bresp(s01_axi_bresp),
        .s_axi_bvalid(s01_axi_bvalid),
        .s_axi_bready(s01_axi_bready),
        .s_axi_araddr(s01_axi_araddr),
        //AWPROT and ARPROT - required by Altera
        //input wire [2 : 0] s_axi_arprot,
        //input wire [2 : 0] s_axi_awprot,
        .s_axi_arvalid(s01_axi_arvalid),
        .s_axi_arready(s01_axi_arready),
        .s_axi_rdata(s01_axi_rdata),
        .s_axi_rresp(s01_axi_rresp),
        .s_axi_rvalid(s01_axi_rvalid),
        .s_axi_rready(s01_axi_rready)
        );
        
   parameter BRP = 2*(`CAN_TIMING0_BRP + 1);
   parameter TS0 = `CAN_TIMING1_TSEG1 + 1;
   parameter TS1 = `CAN_TIMING1_TSEG2 + 1;
   
   parameter CLK_PERIOD = 10;
   
   initial begin
     clk  = 0;
     rstn = 0;
     s00_axi_awaddr = 0;
     s00_axi_awvalid = 0;
     s00_axi_wdata = 0;
     s00_axi_wstrb = 0;
     s00_axi_wvalid = 0;
     s00_axi_bready = 0;
     s00_axi_araddr = 0;
     s00_axi_arvalid = 0;
     s00_axi_rready = 0;
     s01_axi_awaddr = 0;
     s01_axi_awvalid = 0;
     s01_axi_wdata = 0;
     s01_axi_wstrb = 0;
     s01_axi_wvalid = 0;
     s01_axi_bready = 0;
     s01_axi_araddr = 0;
     s01_axi_arvalid = 0;
     s01_axi_rready = 0;
   end
   
   //Generate clock
   always 
   #(CLK_PERIOD/2) clk =  ! clk;
   
   // Generating delayed tx signal (CAN transciever delay)
   reg can_bus_out;
   always @ (o_can0_bus_out or o_can1_bus_out) 
     #(8*BRP) can_bus_out =  o_can0_bus_out & o_can1_bus_out; //8 quants delay
    
    assign i_can0_bus_in = can_bus_out;
    assign i_can1_bus_in = can_bus_out;
    
    reg    extended_mode;
    reg    [31:0] rd_data;
    reg    [31:0] count;
    reg    [31:0] irq_reg;
    reg    [31:0] rd_status;

    initial begin
     extended_mode = 1; 
      #(CLK_PERIOD*20)
      rstn = 1;
      
      ////CAN TEST
      
      //// CAN0 INIT
      #(CLK_PERIOD*10)
      
      // Switch-on reset mode
      write_register00(8'd0, {31'h0, `CAN_MODE_RESET});
            
      // Set bus timing register 0
      write_register00(8'd24, {24'h0, `CAN_TIMING0_SJW, `CAN_TIMING0_BRP});
      // Set bus timing register 1
      write_register00(8'd28, {24'h0, `CAN_TIMING1_SAM, `CAN_TIMING1_TSEG2, `CAN_TIMING1_TSEG1});
      // Set Clock Divider register
      /*
       *extended_mode = clock_divider[7];
       *clock_off     = clock_divider[3];
       *clock divider = clock_divider[2:0];
       */
      write_register00(8'd124, {24'h0, extended_mode, 3'h0, 1'h1, 3'h0});
      
      // Switch-off reset mode
      write_register00(8'd0, {30'h0, ~(`CAN_MODE_RESET)}); // disable dual filtering
            
      //// END CAN0 INIT     

      //// CAN1 INIT
      #(CLK_PERIOD*10);

      // Switch-on reset mode
      write_register01(8'd0, {30'h0, `CAN_MODE_RESET});
      
      // Set bus timing register 0
      write_register01(8'd24, {24'h0, `CAN_TIMING0_SJW, `CAN_TIMING0_BRP});       
      // Set bus timing register 1
      write_register01(8'd28, {24'h0, `CAN_TIMING1_SAM, `CAN_TIMING1_TSEG2, `CAN_TIMING1_TSEG1});    
      // Set Clock Divider register
      write_register01(8'd124, {24'h0, extended_mode, 3'h0, 1'b1, 3'h0});

      // Switch-on filter mode
      write_register01(8'd12, {24'h0, 8'h01});
      
      // Set Acceptance Code and Acceptance Mask registers, to check only 
      // if frame header ID[28] is equal to '1' as a prerequisite to keep it
      write_register01(8'd64, {24'h0, 8'h80}); // acceptance code0
      write_register01(8'd68, {24'h0, 8'h00}); // acceptance code1
      write_register01(8'd72, {24'h0, 8'h00}); // acceptance code2
      write_register01(8'd76, {24'h0, 8'h00}); // acceptance code3
      write_register01(8'd80, {24'h0, 8'h7f}); // acceptance mask0
      write_register01(8'd84, {24'h0, 8'hff}); // acceptance mask1
      write_register01(8'd88, {24'h0, 8'hff}); // acceptance mask2
      write_register01(8'd92, {24'h0, 8'hff}); // acceptance mask3
    
    /********************* REG <-> frame field mapping
      REG_{CODE, MASK}_3[2]  rtr2    (we set mask for rtr2 always '1')
      REG_{CODE, MASK}_3[3]  id[0]   REG_{CODE, MASK}_3[4]  id[1]   
      REG_{CODE, MASK}_3[5]  id[2]   REG_{CODE, MASK}_3[6]  id[3]   
      REG_{CODE, MASK}_3[7]  id[4]   REG_{CODE, MASK}_2[0]  id[5]
      REG_{CODE, MASK}_2[1]  id[6]   REG_{CODE, MASK}_2[2]  id[7]
      REG_{CODE, MASK}_2[3]  id[8]   REG_{CODE, MASK}_2[4]  id[9]
      REG_{CODE, MASK}_2[5]  id[10]  REG_{CODE, MASK}_2[6]  id[11]
      REG_{CODE, MASK}_2[7]  id[12]  REG_{CODE, MASK}_1[0]  id[13]
      REG_{CODE, MASK}_1[1]  id[14]  REG_{CODE, MASK}_1[2]  id[15]
      REG_{CODE, MASK}_1[3]  id[16]  REG_{CODE, MASK}_1[4]  id[17]  
      REG_{CODE, MASK}_1[5]  id[18]  REG_{CODE, MASK}_1[6]  id[19]
      REG_{CODE, MASK}_1[7]  id[20]  REG_{CODE, MASK}_0[0]  id[21]
      REG_{CODE, MASK}_0[1]  id[22]  REG_{CODE, MASK}_0[2]  id[23]
      REG_{CODE, MASK}_0[3]  id[24]  REG_{CODE, MASK}_0[4]  id[25]
      REG_{CODE, MASK}_0[5]  id[26]  REG_{CODE, MASK}_0[6]  id[27]
      REG_{CODE, MASK}_0[7]  id[28]  
     *********************/
      
      // Switch-off filter mode
      write_register01(8'd12, 32'h00);
      
      #(CLK_PERIOD*10);
      
      // Set rx irq on CAN1 (only ext mode)
      write_register01(8'd16, {24'h0, 8'h01});    

      // Switch-off reset mode
      write_register01(8'd0, {24'h0, 7'h4, ~(`CAN_MODE_RESET)}); // disable dual filtering
      
      //// END CAN1 INIT
      
      //// TEST TX, RX
      repeat ((CLK_PERIOD*BRP*(1+TS0+TS1))) @ (posedge clk);   // At least BRP clocks needed before bus goes to dominant level. Otherwise 1 quant difference is possible
      //TX1
      can0_tx_frame(1'b0);
      
      //TX2
      can0_tx_frame(1'b1);

      //RX1
      can1_rx_frame();
            
    end

task write_register00;
  input [7:0] reg_addr;
  input [31:0] reg_data;
begin
//  wait (clk == 1); //#50
//  #(CLK_PERIOD);
@ (posedge clk)
begin
    s00_axi_awaddr  = reg_addr;
    s00_axi_wdata   = reg_data;
    s00_axi_wstrb   = 4'b1111;
    s00_axi_awvalid = 1'b1;
    s00_axi_wvalid  = 1'b1;
    s00_axi_bready = 1'b1;
  end
  
  wait (s00_axi_awready == 1); //#50
  @ (posedge clk)
  begin    
    s00_axi_awvalid = 1'b0;
    s00_axi_wvalid = 1'b0;
  end
  
  wait (s00_axi_bvalid == 1);
    @ (posedge clk)
    begin
      s00_axi_bready = 1'b0;
      if (s00_axi_bresp != 2'b00) begin
        $display("CAN0 write data bad response %d", s00_axi_bresp);
        $finish;
      end
    end
       
  #(2*CLK_PERIOD);
end     
endtask

task write_register01;
  input [7:0] reg_addr;
  input [31:0] reg_data;
begin
  @ (posedge clk)
  begin
    s01_axi_awaddr  = reg_addr;
    s01_axi_wdata   = reg_data;
    s01_axi_wstrb   = 4'b1111;
    s01_axi_awvalid = 1'b1;
    s01_axi_wvalid  = 1'b1;
    s01_axi_bready  = 1'b1;
  end
    
  wait (s01_axi_awready == 1 && clk == 1); //#50
  @ (posedge clk)
  begin
    s01_axi_awvalid = 1'b0;
    s01_axi_wvalid = 1'b0;
  end    

  wait (s01_axi_bvalid == 1);
  @ (posedge clk)
  begin
    s01_axi_bready = 1'b0;
    if (s01_axi_bresp != 2'b00) begin
      $display("CAN0 write data bad response %d", s01_axi_bresp);
      $finish;
    end
  end
  #(2*CLK_PERIOD);
end     
endtask

task read_register00;
  input  [7:0] reg_addr;
  output [31:0] data_o;
begin
  wait (clk == 0) //#25
    s00_axi_araddr  = reg_addr;
    s00_axi_arvalid = 1'b1;
    s00_axi_rready  = 1'b1;
  
  wait (s00_axi_arready == 1) //#25
    #CLK_PERIOD
      s00_axi_arvalid = 1'b0;
  
  wait (s00_axi_rvalid == 1) //#25
    #CLK_PERIOD
      data_o         = s00_axi_rdata;
      s00_axi_rready = 1'b0;
      if (s00_axi_rresp !== 2'b00) begin
        $display("CAN0 read data bad response %d", s00_axi_rresp);
        $finish;
      end
  
end
endtask

task read_register01;
  input  [7:0] reg_addr;
  output [31:0] data_o;
begin
wait (clk == 0) //#25
  s01_axi_araddr  = reg_addr;
  s01_axi_arvalid = 1'b1;
  s01_axi_rready  = 1'b1;

wait (s01_axi_arready == 1) //#25
  #CLK_PERIOD
    s01_axi_arvalid = 1'b0;

wait (s01_axi_rvalid == 1) //#25
  #CLK_PERIOD
    data_o         = s01_axi_rdata;
    s01_axi_rready = 1'b0;
    if (s01_axi_rresp !== 2'b00) begin
      $display("CAN1 read data bad response %d", s01_axi_rresp);
      $finish;
    end

end
endtask

task can0_tx_frame;
  input reg [0:0] id;
begin
  
  //Ensure that controller is not transmitting 
  read_register00(8'd8, rd_status);
  while(rd_status[2] == 0)
  begin
    #((CLK_PERIOD*BRP*(1+TS0+TS1))*10);
    read_register00(8'd8, rd_status);
  end
  
  // Extended frame format
  // Writing TX frame information + identifier + data
  write_register00(8'd64, {4'h8, 4'h2});  // Frame format = 1(extended), Remote transmision request = 0, DLC = 2
  write_register00(8'd68, {id, 7'h20});  // ID[28:21]
  write_register00(8'd72, {24'h0, 8'h00});  // ID[20:13]
  write_register00(8'd76, {24'h0, 8'h00});  // ID[12:5]
  write_register00(8'd80, {24'h0, 8'h00});  // ID[4:0]
  write_register00(8'd84, {24'h0, 8'h03});  //payload0
  write_register00(8'd88, {24'h0, 8'hc6});  //payload1
//  write_register00(8'd92, 8'h54);  //payload2
//  write_register00(8'd96, 8'h16);  //payload3
//  write_register00(8'd100, 8'hf4);  //payload4
//  write_register00(8'd104, 8'ha2);  //payload5
//  write_register00(8'd108, 8'h9d);  //payload6
//  write_register00(8'd112, 8'he1);  //payload7

  //Order TX
  write_register00(8'd4, {24'h0, 8'h1});
end
endtask

task can1_rx_frame;
begin
    
  // Wait for interrupt
  while(o_irq1 == 0)
  begin
    #(CLK_PERIOD);
  end

  #(CLK_PERIOD*10);

  //Read irq reg
  read_register01(8'd12, irq_reg);
  $display("CAN1 IRQ reg 0x%x", irq_reg); //This should indicate RX interrupt, i.e., value = 0x1

  //Read RX fifo frames count
  read_register01(8'd116, count);  
  $display("remain frames 0x%x", count);
  
  //Start reading frame
  read_register01(8'd64, rd_data);
  #CLK_PERIOD
  $display("rx_buff0 0x%x", rd_data);
  read_register01(8'd68, rd_data);
  #CLK_PERIOD
  $display("rx_buff1 0x%x", rd_data);
  read_register01(8'd72, rd_data);
  #CLK_PERIOD
  $display("rx_buff2 0x%x", rd_data);
  read_register01(8'd76, rd_data);
  #CLK_PERIOD
  $display("rx_buff3 0x%x", rd_data);
  #CLK_PERIOD
  read_register01(8'd80, rd_data);
  $display("rx_buff4 0x%x", rd_data);
  read_register01(8'd84, rd_data);
  #CLK_PERIOD
  $display("rx_buff5 0x%x", rd_data);
  read_register01(8'd88, rd_data);
  #CLK_PERIOD
  $display("rx_buff6 0x%x", rd_data);
  read_register01(8'd92, rd_data);
  #CLK_PERIOD
  $display("rx_buff7 0x%x", rd_data);
  read_register01(8'd96, rd_data);
  #CLK_PERIOD
  $display("rx_buff8 0x%x", rd_data);
  read_register01(8'd100, rd_data);
  #CLK_PERIOD
  $display("rx_buff9 0x%x", rd_data);
  read_register01(8'd104, rd_data);
  #CLK_PERIOD
  $display("rx_buff10 0x%x", rd_data);
  read_register01(8'd108, rd_data);
  #CLK_PERIOD
  $display("rx_buff11 0x%x", rd_data);
  read_register01(8'd112, rd_data);
  #CLK_PERIOD
  $display("rx_buff12 0x%x", rd_data);
  
  //Acknowledge RX read
  write_register01(8'd8, {24'h0, 8'h4});

  //Read RX fifo frames count
  read_register01(8'd116, count);  
  $display("remain frames 0x%x", count);
  
  // clear IRQ 
  read_register01(8'd4, rd_data);
  
end
endtask

endmodule
