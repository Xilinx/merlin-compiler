
############################
# Project settings

variable module [exec basename [pwd]] 
variable version [exec basename $argv0] 
regsub -all {.tcl} $version {} version

cd projects
open_project $version.prj

add_files ../$module.c
#add_files ../$version.c -tb
#add_files ../polybench.c -tb
#regsub -all {\-} $module {_} module
set_top compute1
#############################
#Solution settings
open_solution solution1
#
set_part xc7k325tfbg676-1
set_part virtex7
#set_part virtex6_fpv5
create_clock -period 5.0ns
set_clock_uncertainty 1.25
#
#Synthesis
csynth_design
file copy -force $version.prj/solution1/syn/report/kernel_${module}_csynth.rpt ../report/HLS_syn.rpt 


