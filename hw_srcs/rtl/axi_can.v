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
 * |-- Created    : 2017-01-24
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

`include "can_defines.v"
`include "timescale.v"

module axi_can(
    // CAN Bus ports
    input wire  i_can_bus_in,
    output wire o_can_bus_out,
    output wire o_irq,
    output wire can_clkout_o,
    // Ports of Axi Slave Bus Interface
    input wire  s_axi_aclk,
    input wire  s_axi_aresetn,
    input wire [7 : 0] s_axi_awaddr,
    input wire  s_axi_awvalid,
    output wire  s_axi_awready,
    input wire [31 : 0] s_axi_wdata,
    input wire [3 : 0] s_axi_wstrb,
    input wire  s_axi_wvalid,
    output wire  s_axi_wready,
    output wire [1 : 0] s_axi_bresp,
    output wire  s_axi_bvalid,
    input wire  s_axi_bready,
    input wire [7 : 0] s_axi_araddr,
    //AWPROT and ARPROT - required by Altera
//    input wire [2 : 0] s_axi_arprot,
//    input wire [2 : 0] s_axi_awprot,
    input wire  s_axi_arvalid,
    output wire  s_axi_arready,
    output wire [31 : 0] s_axi_rdata,
    output wire [1 : 0] s_axi_rresp,
    output wire  s_axi_rvalid,
    input wire  s_axi_rready
);

   wire wb_clk_o  /*synthesis keep=1 */;
   wire wb_rst_o  /*synthesis keep=1 */;
   wire [7:0]wb_addr_o /*synthesis keep=1 */;
   wire [31:0]wb_dat_o  /*synthesis keep=1 */;
   wire wb_we_o   /*synthesis keep=1 */;
   wire wb_stb_o  /*synthesis keep=1 */;
   wire wb_cyc_o  /*synthesis keep=1 */;
   wire [31:0]wb_dat_i  /*synthesis keep=1 */;
   wire wb_err_i  /*synthesis keep=1 */;
   wire wb_ack_i  /*synthesis keep=1 */;
   
   wire can_bus_off_on;
   
   axil2wb #(8,32) axil2wb 
   (
   .S_AXI_ACLK    (s_axi_aclk),
   .S_AXI_ARESETN (s_axi_aresetn),
   .S_AXI_AWADDR  (s_axi_awaddr),
   .S_AXI_AWVALID (s_axi_awvalid),
   .S_AXI_AWREADY (s_axi_awready),
   .S_AXI_WDATA   (s_axi_wdata),
   .S_AXI_WSTRB   (s_axi_wstrb),
   .S_AXI_WVALID  (s_axi_wvalid),
   .S_AXI_WREADY  (s_axi_wready),
   .S_AXI_ARADDR  (s_axi_araddr),
   .S_AXI_ARVALID (s_axi_arvalid),
   .S_AXI_ARREADY (s_axi_arready),
   .S_AXI_RDATA   (s_axi_rdata),
   .S_AXI_RRESP   (s_axi_rresp),
   .S_AXI_RVALID  (s_axi_rvalid),
   .S_AXI_RREADY  (s_axi_rready),
   .S_AXI_BRESP   (s_axi_bresp),
   .S_AXI_BVALID  (s_axi_bvalid),
   .S_AXI_BREADY  (s_axi_bready),
   //AWPROT and ARPROT - required by Altera
   //S_AXI_ARPROT  : in  std_logic_vector(2 downto 0);
   //S_AXI_AWPROT  : in  std_logic_vector(2 downto 0);
   .wb_clk_o      (wb_clk_o),
   .wb_rst_o      (wb_rst_o),
   .wb_addr_o     (wb_addr_o),
   .wb_dat_o      (wb_dat_o),
   .wb_we_o       (wb_we_o),
   .wb_stb_o      (wb_stb_o),
   .wb_cyc_o      (wb_cyc_o),
   .wb_dat_i      (wb_dat_i),
   .wb_err_i      (wb_err_i),
   .wb_ack_i      (wb_ack_i)
   ); 
   
   wire o_irq_r;
   assign o_irq = ~o_irq_r;

   can_top i_can_top
   ( 
     .wb_clk_i(wb_clk_o),
     .wb_rst_i(wb_rst_o),
     .wb_dat_i(wb_dat_o),
     .wb_dat_o(wb_dat_i),
     .wb_cyc_i(wb_cyc_o),
     .wb_stb_i(wb_stb_o),
     .wb_we_i(wb_we_o),
     .wb_addr_i(wb_addr_o),
     .wb_ack_o(wb_ack_i),
     .wb_err_o(wb_err_i),
     .clk_i(s_axi_aclk),
     .rx_i(i_can_bus_in),
     .tx_o(o_can_bus_out),
     .bus_off_on(can_bus_off_on),
     .irq_on(o_irq_r),
     .clkout_o(can_clkout_o)
     
     // Bist
   `ifdef CAN_BIST
     ,
     // debug chain signals
     .mbist_si_i(1'b0),       // bist scan serial in
     .mbist_so_o(),           // bist scan serial out
     .mbist_ctrl_i(3'b001)    // mbist scan {enable, clock, reset}
   `endif
   );
   
endmodule
