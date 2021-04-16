# Xilinx Vivado TCL script to config the FPGA with the bitstream.
# Author: Luca Pezzarossa (lpez@dtu.dk)
open_project ./hardware/vivado/nexys4ddr/nexys4ddr.xpr
open_hw
connect_hw_server
# Maybe the following line is not corret for all PCs
open_hw_target
set_property PROGRAM.FILE {./hardware/vivado/nexys4ddr/nexys4ddr.runs/impl_1/patmos_top.bit} [lindex [get_hw_devices xc7a100t_0] 0]
current_hw_device [lindex [get_hw_devices xc7a100t_0] 0]
refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xc7a100t_0] 0]
set_property PROBES.FILE {} [lindex [get_hw_devices xc7a100t_0] 0]
set_property PROGRAM.FILE {./hardware/vivado/nexys4ddr/nexys4ddr.runs/impl_1/patmos_top.bit} [lindex [get_hw_devices xc7a100t_0] 0]
program_hw_devices [lindex [get_hw_devices xc7a100t_0] 0]
refresh_hw_device [lindex [get_hw_devices xc7a100t_0] 0]
close_hw
close_project
