transcript on
if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

vlog -sv -work work +incdir+C:/Users/strisorus/Downloads/e155-final-master/e155-final-master/fpga_peripherals {C:/Users/strisorus/Downloads/e155-final-master/e155-final-master/fpga_peripherals/peripherals.sv}

