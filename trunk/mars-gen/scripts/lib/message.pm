
#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
my $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
package message;
use strict;
use warnings;

my $MODULE   = "MERCC";
my $ERROR   = "ERROR";
my $WARNING = "WARNING";
my $INFO    = "INFO";
my $merlin_log = "merlin.log";
my $support = "support\@falcon-computing.com";

# for each message
# 000~199 : Merlin related message
# 200~339 : Xilinx related message
# 400~559 : Intel related message
# 600~779 : Msg_report related message
# 800-899 : DSE related message
use Exporter 'import';
our @ISA = 'Exporter';
our @EXPORT = qw(
        MSG_I_1000
        MSG_I_1001
        MSG_I_1002
        MSG_I_1003
        MSG_I_1004
        MSG_I_1005
        MSG_I_1006
        MSG_I_1007
        $MSG_I_1008
        $MSG_I_1009
        $MSG_I_1010
        $MSG_I_1011
        $MSG_I_1012
        $MSG_I_1013
        $MSG_I_1014
        $MSG_I_1015
        MSG_I_1016
        MSG_I_1017
        MSG_I_1018
        MSG_I_1019
        MSG_I_1020
        MSG_I_1021
        MSG_I_1022
        MSG_I_1023
        MSG_I_1024
        MSG_I_1025
        MSG_I_1026
        MSG_I_1027
        MSG_I_1028
        MSG_I_1029
        MSG_I_1030
        MSG_I_1031
        MSG_I_1032
        $MSG_I_1033
        $MSG_I_1034
        $MSG_I_1035
        $MSG_I_1036
        $MSG_I_1037
        MSG_I_1038
        MSG_I_1039
        MSG_I_1040
        MSG_I_1041
        $MSG_I_1042
        $MSG_I_1043
        $MSG_I_1044
        $MSG_I_1045
        $MSG_I_1046
        $MSG_I_1047
        MSG_I_1095
        MSG_I_1096
        MSG_I_1097
        MSG_I_1098
        MSG_I_1099
        MSG_I_1201

        MSG_W_2000
        MSG_W_2001
        MSG_W_2002
        MSG_W_2003
        MSG_W_2004
        MSG_W_2005
        MSG_W_2006
        MSG_W_2007
        MSG_W_2008
        MSG_W_2009
        MSG_W_2010
        MSG_W_2201

        MSG_E_3000
        MSG_E_3001
        MSG_E_3002
        MSG_E_3003
        MSG_E_3004
        MSG_E_3005
        MSG_E_3006
        MSG_E_3007
        MSG_E_3008
        MSG_E_3009
        MSG_E_3010
        MSG_E_3011
        MSG_E_3012
        MSG_E_3013
        MSG_E_3014
        MSG_E_3015
        MSG_E_3016
        MSG_E_3017
        MSG_E_3018
        $MSG_E_3019
        $MSG_E_3020
        $MSG_E_3021
        $MSG_E_3022
        MSG_E_3023
        MSG_E_3024
        MSG_E_3025
        MSG_E_3026
        MSG_E_3027
        MSG_E_3028
        MSG_E_3029
        MSG_E_3030
        MSG_E_3031
        MSG_E_3032
        MSG_E_3033
        MSG_E_3034
        MSG_E_3035
        MSG_E_3036
        MSG_E_3037
        MSG_E_3038
        MSG_E_3039
        MSG_E_3040
        MSG_E_3041
        MSG_E_3042
        MSG_E_3043
        MSG_E_3044
        MSG_E_3045
        MSG_E_3046
        MSG_E_3047
        MSG_E_3048
        MSG_E_3049
        MSG_E_3050
        MSG_E_3051
        MSG_E_3052
        MSG_E_3053
        MSG_E_3054
        MSG_E_3055
        MSG_E_3056
        $MSG_E_3057
        $MSG_E_3058
        $MSG_E_3059
        $MSG_E_3060
        MSG_E_3061
        MSG_E_3062
        MSG_E_3063
        MSG_E_3064
        MSG_E_3065
        $MSG_E_3066
        MSG_E_3067
        MSG_E_3068
        MSG_E_3069
        MSG_E_3070
        MSG_E_3071
        MSG_E_3072
        $MSG_E_3073
        MSG_E_3074
        MSG_E_3075
        MSG_E_3076
        MSG_E_3077
        $MSG_E_3078
        $MSG_E_3079
        $MSG_E_3080
        MSG_E_3081
        MSG_E_3082
        MSG_E_3083
        MSG_E_3084
        MSG_E_3085
        MSG_E_3086
        MSG_E_3087
        MSG_E_3088
        MSG_E_3089
        MSG_E_3090
        MSG_E_3091
        MSG_E_3092
        MSG_E_3093
        MSG_E_3094
        MSG_E_3095
        MSG_E_3096
        MSG_E_3097
        MSG_E_3098
        MSG_E_3099
        MSG_E_3199

        MSG_E_3200
        MSG_E_3201
        MSG_E_3202
        MSG_E_3203

        MSG_E_3400
        MSG_E_3401
        MSG_E_3402
        $MSG_E_3403
    );
our (
        $MSG_I_1008,
        $MSG_I_1009,
        $MSG_I_1010,
        $MSG_I_1011,
        $MSG_I_1012,
        $MSG_I_1013,
        $MSG_I_1014,
        $MSG_I_1015,
        $MSG_I_1033,
        $MSG_I_1034,
        $MSG_I_1035,
        $MSG_I_1036,
        $MSG_I_1037,
        $MSG_I_1042,
        $MSG_I_1043,
        $MSG_I_1044,
        $MSG_I_1045,
        $MSG_I_1046,
        $MSG_I_1047,
        $MSG_E_3019,
        $MSG_E_3020,
        $MSG_E_3021,
        $MSG_E_3022,
        $MSG_E_3057,
        $MSG_E_3058,
        $MSG_E_3059,
        $MSG_E_3060,
        $MSG_E_3066,
        $MSG_E_3072,
        $MSG_E_3073,
        $MSG_E_3078,
        $MSG_E_3079,
        $MSG_E_3080,
        $MSG_E_3403
    );

sub print_log {
    my $string = $_[0];
    print $string;
    open(my $fh, '>>', $merlin_log) or die "Could not open file '$merlin_log' $!";
    print $fh $string;
    close $fh;
} 

#information
sub MSG_I_1000 {
    print "$INFO: [$MODULE-1000] This is a testing information\n";
}
sub MSG_I_1001 {}
sub MSG_I_1002 {}
sub MSG_I_1003 {
    print "$INFO: [$MODULE-1003] Merlin library generated.\n";
}
sub MSG_I_1004 {
    print "$INFO: [$MODULE-1004] Host program generated.\n";
}
sub MSG_I_1005 {
    print_log "$INFO: [$MODULE-1005] Checking license for $_[0]...\n";
}
sub MSG_I_1006 {
    print_log "$INFO: [$MODULE-1006] Did not find $_[0] license.\n";
}
sub MSG_I_1007 {
    print_log "$INFO: [$MODULE-1007] $_[0]";
}
$MSG_I_1008 = "$INFO: [$MODULE-1008] Checking Xilinx xocc ...\n";
$MSG_I_1009 = "$INFO: [$MODULE-1009] Found Xilinx xocc ";
$MSG_I_1010 = "$INFO: [$MODULE-1010] Checking Intel aoc ...\n";
$MSG_I_1011 = "$INFO: [$MODULE-1011] Found Intel aoc ";
$MSG_I_1012 = "$INFO: [$MODULE-1012] Checking Intel quartus ...\n";
$MSG_I_1013 = "$INFO: [$MODULE-1013] Found Intel quartus ";
$MSG_I_1014 = "$INFO: [$MODULE-1014] Checking Intel AOCL_BOARD_PACKAGE_ROOT ...\n";
$MSG_I_1015 = "$INFO: [$MODULE-1015] Found Intel AOCL_BOARD_PACKAGE_ROOT ";
sub MSG_I_1016 {
    print_log "$INFO: [$MODULE-1016] Please execute \'merlincc\' to check legal input argument format.\n";
}
sub MSG_I_1017 {
    print "\n$INFO: [$MODULE-1017] Target device: $_[0].\n";
}
sub MSG_I_1018 {
    my $kernel = "";
    if($_[0] ne "") { $kernel = $_[0]; }
    print "\n$INFO: [$MODULE-1018] Compiling kernel $_[0]...\n";
}
sub MSG_I_1019 {
    print "$INFO: [$MODULE-1019] Linking library...\n";
}
sub MSG_I_1020 {
    print "$INFO: [$MODULE-1020] Generating software emulation configuration file...\n";
}
sub MSG_I_1021 {
    print "$INFO: [$MODULE-1021] Generating hardware emulation configuration file...\n";
}
sub MSG_I_1022 {
    print "$INFO: [$MODULE-1022] Generating hardware configuration file...\n";
}
sub MSG_I_1023 {
    print "$INFO: [$MODULE-1023] Running code parser...\n";
}
sub MSG_I_1024 {
    print "\n$INFO: [$MODULE-1024] Report generating...\n";
}
sub MSG_I_1025 {
    print "$INFO: [$MODULE-1025] High-level design estimating ...\n";
}
sub MSG_I_1026 {
    print "$INFO: [$MODULE-1026] Estimation successfully.\n";
}
sub MSG_I_1027 {
    print "$INFO: [$MODULE-1027] Software emulation configuration file generation ...\n";
}
sub MSG_I_1028 {
    print "\n$INFO: [$MODULE-1028] Software emulation configuration file generated successfully.\n";
}
sub MSG_I_1029 {
    print "$INFO: [$MODULE-1029] Hardware emulation configuration file generation ...\n";
}
sub MSG_I_1030 {
    print "\n$INFO: [$MODULE-1030] Hardware emulation configuration file generated successfully.\n";
}
sub MSG_I_1031 {
    print "$INFO: [$MODULE-1031] Hardware configuration file generation ...\n";
}
sub MSG_I_1032 {
    print "\n$INFO: [$MODULE-1032] Hardware configuration file generated successfully.\n";
}
$MSG_I_1033 = "$INFO: [$MODULE-1033] Syntax checking";
$MSG_I_1034 = "$INFO: [$MODULE-1034] Synthesizability checking";
$MSG_I_1035 = "$INFO: [$MODULE-1035] Interface synthesis";
$MSG_I_1036 = "$INFO: [$MODULE-1036] Kernel optimization";
$MSG_I_1037 = "$INFO: [$MODULE-1037] Code generation";
sub MSG_I_1038 {
    print_log "$INFO: [$MODULE-1038] Xilinx support platform:\n";
}
sub MSG_I_1039 {
    print_log "$INFO: [$MODULE-1039] Intel support platform:\n";
}
sub MSG_I_1040 {
    print "\n$INFO: [$MODULE-1040] Compilation finished successfully.\n";
}
sub MSG_I_1041 {
    print_log "$INFO: [$MODULE-1041] Setting platform to $_[0].\n";
}
$MSG_I_1042 = "$INFO: [$MODULE-1042] Checking Xilinx v++ ...\n";
$MSG_I_1043 = "$INFO: [$MODULE-1043] Found Xilinx v++ ";
$MSG_I_1044 = "$INFO: [$MODULE-1044] Checking Xilinx Vitis HLS ...\n";
$MSG_I_1045 = "$INFO: [$MODULE-1045] Found Xilinx Vitis HLS ";
$MSG_I_1046 = "$INFO: [$MODULE-1046] Checking Xilinx Vivado HLS ...\n";
$MSG_I_1047 = "$INFO: [$MODULE-1047] Found Xilinx Vivado HLS ";
sub MSG_I_1095 {
    print "$INFO: [$MODULE-1095] Executing in non-encrypt mode.\n";
}
sub MSG_I_1096 {
    return "$INFO: [$MODULE-1096] Host - '$_[0]' with $_[1] $_[2]\n";
}
sub MSG_I_1097 {
    return "$INFO: [$MODULE-1097] User - $_[0]\n";
}
sub MSG_I_1098 {
    return "$INFO: [$MODULE-1098] Time - $_[0]\n";
}
sub MSG_I_1099 {
    return "$INFO: [$MODULE-1099] Directory - $_[0]\n\n";
}
sub MSG_I_1201 {
    print "$INFO: [$MODULE-1201] Pre-check sdaccel license ...\n";
}

#warning
sub MSG_W_2000 {
    print "$WARNING: [$MODULE-2000] This is a testing information\n";
}
sub MSG_W_2001 {
    my $string = "$WARNING: [$MODULE-2001] Overwrite $_[0]. Use --appendlog option to append $_[0].\n";
    print_log "$string";
    return $string;
}
sub MSG_W_2002 {
    my ($require_space, $current_space) = @_;
    print_log "\n$WARNING: [$MODULE-2002] Not enough space (required $require_space KB, current $current_space KB). \n";
}
sub MSG_W_2003 {
    print_log "$WARNING: [$MODULE-2003] Fast DSE not successful.\n";
}
sub MSG_W_2201 {
    print "\n$WARNING: [$MODULE-2201] Did not generate emconfig.json file. \n";
}

#error
sub MSG_E_3000 {
    print "$ERROR: [$MODULE-3000] This is a testing information\n";
}
sub MSG_E_3001 {
    my ($impl_tool) = @_;
    print_log "$ERROR: [$MODULE-3001] Fail to generate the host API library!\n";
    if($impl_tool ~~ ["vitis", "sdaccel", "vivado_hls", "vitis_hls"]) {
        print_log "\tPlease make sure OpenCL headers are installed and environment variable XILINX_XRT points at the XILINX XRT root.\n";
    } else {
        print_log "\tPlease make sure OpenCL headers are installed.\n";
    }
}
sub MSG_E_3002 {
    print_log "$ERROR: [$MODULE-3002] Can not find kernel_file.json\n";
}
sub MSG_E_3003 {
    print "$ERROR: [$MODULE-3003] Fail to generate Merlin library.\n";
}
sub MSG_E_3004 {
    print_log "$ERROR: [$MODULE-3004] Fail to generate host program.\n";
}
sub MSG_E_3005 {
    print "\n$ERROR: [$MODULE-3005] Fail to estimate.\n";
}
sub MSG_E_3006 {
    print_log "$ERROR: [$MODULE-3006] Fail to generate estimation report.\n";
}
sub MSG_E_3007 {
    print "$ERROR: [$MODULE-3007] Please specify server name by --attribute server=xxxx.\n";
}
sub MSG_E_3008 {
    print "$ERROR: [$MODULE-3008] Do not support more than one $_[0] argument.\n";
}
sub MSG_E_3009 {
    print "$ERROR: [$MODULE-3009] Invalid flow \'$_[0]\'.\n";
}
sub MSG_E_3010 {
    print "$ERROR: [$MODULE-3010] Directory $_[0] not exist.\n";
}
sub MSG_E_3011 {
    print_log "$ERROR: [$MODULE-3011] Miss value of option $_[0].\n";
}
sub MSG_E_3012 {
    print "$ERROR: [$MODULE-3012] Can not find directive.xml.\n";
}
sub MSG_E_3013 {
    print "$ERROR: [$MODULE-3013] --attribute must followed with x=x or x+=x, x can not be empty.\n";
}
sub MSG_E_3014 {
    print "$ERROR: [$MODULE-3014] To run vendor tool, the input directory must contain kernel, lc, host and lib_gen.\n";
}
sub MSG_E_3015 {
    print "$ERROR: [$MODULE-3015] Do not support option $_[0].\n";
}
sub MSG_E_3016 {
    print "$ERROR: [$MODULE-3016] Necessary project file $_[0] not exist.\n";
}
sub MSG_E_3017 {
    print_log "$ERROR: [$MODULE-3017] $_[0] licensing checkout failed.\n";
}
sub MSG_E_3018 {
    print "$ERROR: [$MODULE-3018] $_[0] .\n";
}
$MSG_E_3019 = "$ERROR: [$MODULE-3019] Did not find Xilinx xocc, Please set PATH point to xocc directory.\n";
$MSG_E_3020 = "$ERROR: [$MODULE-3020] Did not find Intel aoc, Please set Intel aoc environment correctly.\n";
$MSG_E_3021 = "$ERROR: [$MODULE-3021] Did not find Intel quartus, Please set Intel quartus environment correctly.\n";
$MSG_E_3022 = "$ERROR: [$MODULE-3022] Did not find Intel AOCL_BOARD_PACKAGE_ROOT, Please set Intel AOCL_BOARD_PACKAGE_ROOT environment correctly.\n";
sub MSG_E_3023 {
    print_log "$ERROR: [$MODULE-3023] Do not support multiple optimization effort. $_[0] conflict.\n";
}
sub MSG_E_3024 {
    print_log "$ERROR: [$MODULE-3024] Kernel frequency must be positive integer.\n";
}
sub MSG_E_3025 {
    print_log "$ERROR: [$MODULE-3025] Unknown implementation tool: '$_[0]'.\n";
    print_log "The valid implementation tools are sdaccel/vitis/vitis_hls/vivado_hls.\n";
}
sub MSG_E_3026 {
    print_log "$ERROR: [$MODULE-3026] Conflict option $_[0] value, $_[1] with $_[2].\n";
}
sub MSG_E_3027 {
    print_log "$ERROR: [$MODULE-3027] Error usage of --platform or -p:\n";
    print_log "  usage : --platform=implementation_tool::platform_name\n";
    print_log "          -p=implementation_tool::platform_name\n";
}
sub MSG_E_3028 {
    print_log "$ERROR: [$MODULE-3028] Error usage of -march:\n";
    print_log "  usage : -march=[sw_emu|hw_emu]\n";
}
sub MSG_E_3029 {
    print_log "$ERROR: [$MODULE-3029] Error usage of --report:\n";
    print_log "  usage : --report=estimate\n";
}
sub MSG_E_3030 {
    print_log "$ERROR: [$MODULE-3030] Illegal input argument \'$_[0]\'.\n";
}
sub MSG_E_3031 {
    print_log "$ERROR: [$MODULE-3031] File $_[0] not exist.\n";
}
sub MSG_E_3032 {
    print_log "$ERROR: [$MODULE-3032] Filename $_[0] is illegal.\n";
}
sub MSG_E_3033 {
    print_log "$ERROR: [$MODULE-3033] No source file exist, please input source code or mco file.\n";
}
sub MSG_E_3034 {
    print_log "$ERROR: [$MODULE-3034] Conflict source file, please don't input source code and mco file at the same file.\n";
}
sub MSG_E_3035 {
    print_log "$ERROR: [$MODULE-3035] Attribute \'$_[0]\' is not valid.\n";
}
sub MSG_E_3036 {
    print "$ERROR: [$MODULE-3036] Xilinx support platform:\n";
}
sub MSG_E_3037 {
    print_log "$ERROR: [$MODULE-3037] Unknown target platform: \'$_[0]\'. Available ones are:\n";
    print_log "$_[1]\n";
}
sub MSG_E_3038 {
    print_log "$ERROR: [$MODULE-3038] Please use --platform or -p to specify platform.\n";
}
sub MSG_E_3039 {
    print_log "$ERROR: [$MODULE-3039] Please remove $_[0] before running merlincc.\n";
}
sub MSG_E_3040 {
    print_log "$ERROR: [$MODULE-3040] Did not find $_[0] in current directory.\n";
}
sub MSG_E_3041 {
    print_log "$ERROR: [$MODULE-3041] Input shouldn't be .mco file, if '-c' flag is specified.\n";
}
sub MSG_E_3042 {
    print_log "$ERROR: [$MODULE-3042] Check license failed.\n";
}
sub MSG_E_3043 {
    print_log "$ERROR: [$MODULE-3043] Directory $_[0] not exist\n";
}
sub MSG_E_3044 {
    print_log "$ERROR: [$MODULE-3044] Merlin compilation failed. See above for each details.\n";
}
sub MSG_E_3045 {
    print_log "$ERROR: [$MODULE-3045] Merlin Compiler Object file generated for different FPGA vendor.\n";
    print_log "  Please recompile using 'merlincc -c'.\n";
}
sub MSG_E_3046 {
}
sub MSG_E_3047 {
    print_log "$ERROR: [$MODULE-3047] Fail to generate simulation configuration file.\n";
}
sub MSG_E_3048 {
    print_log "$ERROR: [$MODULE-3048] Fail to generate hardware configuration file.\n";
}
sub MSG_E_3049 {
    print_log "$ERROR: [$MODULE-3049] Fail to generate implementation report.\n";
}
sub MSG_E_3050 {
    print "$ERROR: [$MODULE-3050] Executing in non-encrypt mode.\n";
}
sub MSG_E_3051 {
    print "\n$ERROR: [$MODULE-3051] Estimation failed.\n";
}
sub MSG_E_3052 {
    print "\n$ERROR: [$MODULE-3052] Software emulation configuration file generation failed.\n";
}
sub MSG_E_3053 {
    print "\n$ERROR: [$MODULE-3053] Hardware emulation configuration file generation failed.\n";
}
sub MSG_E_3054 {
    print "\n$ERROR: [$MODULE-3054] Hardware configuration file generation failed.\n";
}
sub MSG_E_3055 {
    print "\n$ERROR: [$MODULE-3055] Compilation failed.\n";
}
sub MSG_E_3056 {
    print "\n$ERROR: [$MODULE-3056] Synthesizability check failed.\nHint: only c++03 is supported now.\n";
}
$MSG_E_3057 = "\n$ERROR: [$MODULE-3057] Syntax check failed.\nHint: only c++03 or c99 is supported now.\n";
$MSG_E_3058 = "\n$ERROR: [$MODULE-3058] Frontend compilation failed, please see the details in merlin.log";
$MSG_E_3059 = "\n$ERROR: [$MODULE-3059] Merlin Compiler exits due to previous error(s).\n";
$MSG_E_3060 = "\n$ERROR: [$MODULE-3060] Merlin flow stopped with error(s).\n";
sub MSG_E_3061 {
    print "\n$ERROR: [$MODULE-3061] cmd=$_[0]\n  pwd=$_[1]";
}
sub MSG_E_3062 {
    print "$ERROR: [$MODULE-3062] Did not find platform directory in $_[0]\n";
}
sub MSG_E_3063 {
    print_log "$ERROR: [$MODULE-3063] Did not find zip. Please use command \'yum install -y zip\' to install.\n";
}
sub MSG_E_3064 {
    print_log "$ERROR: [$MODULE-3064] Did not find unzip. Please use command \'yum install -y unzip\' to install.\n";
}
sub MSG_E_3065 {
    print_log "$ERROR: [$MODULE-3065] Did not find tar. Please use command \'yum install -y tar\' to install.\n";
}
$MSG_E_3066 = "$ERROR: [$MODULE-3066] Intel environment \'AOCL_BOARD_PACKAGE_ROOT\' not set correctly:\n";
sub MSG_E_3067 {
    print_log "$ERROR: [$MODULE-3067] Did not find lsb_release. Please use command \'yum install redhat-lsb-core\' to install.\n";
}
sub MSG_E_3068 {
    print_log "$ERROR: [$MODULE-3068] Did not find python. Please install Python 2.7.x.\n";
}
sub MSG_E_3069 {
    print_log "$ERROR: [$MODULE-3069] Current Merlin only support Python 2.7.x.\n";
    print_log "  But the current using python version is $_[0].\n";
}
sub MSG_E_3070 {
    print_log "$ERROR: [$MODULE-3070] File path/name '$_[0]' contains illegal character ' '.\n";
}
sub MSG_E_3071 {
    print_log "$ERROR: [$MODULE-3071] Current directory path '$_[0]' contains illegal character ' '.\n";
}
sub MSG_E_3072 {
    print_log "$ERROR: [$MODULE-3072] Did not find valid XILINX_XRT environment.\n";
}
$MSG_E_3073 = "$ERROR: [$MODULE-3073] Did not find Xilinx v++, Please set PATH point to v++ directory.\n";
sub MSG_E_3074 {
    print_log "$ERROR: [$MODULE-3074] Target platform \'$_[0]\' does not exist.\n";
}
sub MSG_E_3075 {
    print_log "$ERROR: [$MODULE-3075] Found relative path for xpfm file: \'$_[0]\'.\n";
    print_log "  Please use absolute path instead.\n";
}
sub MSG_E_3076 {
    print_log "$ERROR: [$MODULE-3076] Invalid attribute usage: \'$_[0]\'.\n";
    print_log "  Correct usage: --attribute key=value.\n";
}
sub MSG_E_3077 {
    print_log "$ERROR: [$MODULE-3077] Dash shell is not currently supported. Please change the default shell to Bash: \'sudo ln -sf /bin/bash /bin/sh\'.\n";
}
$MSG_E_3078 = "$ERROR: [$MODULE-3078] Did not find Xilinx Vitis HLS, Please set PATH point to vitis_hls directory.\n";
$MSG_E_3079 = "$ERROR: [$MODULE-3079] Did not find Xilinx Vivado HLS, Please set PATH point to vivado_hls directory.\n";
$MSG_E_3080 = "$ERROR: [$MODULE-3080] Please contact '${support}' to acquire a proper Merlin license.\n";
sub MSG_E_3200 {
    print "$ERROR: [$MODULE-3200] OpenCL simulation is disabled for C designs with wide bus support activated.\n";
    print "  Please consider to move the design to C++.\n";
}
sub MSG_E_3201 {
    print "$ERROR: [$MODULE-3201] Quit Xilinx SDx flow, \$HOME environment is not set!\n";
    print "  Please set \$HOME environment\n";
}
sub MSG_E_3202 {
    print_log "$ERROR: [$MODULE-3202] merlincc only support 2016.3 and later version for SDx\n";
}
sub MSG_E_3400 {
    print "\n$ERROR: [$MODULE-3400] $_[0]\n";
}

sub MSG_E_3401 {
    print_log "\n$ERROR: [$MODULE-3401] XPFM file $_[0] not exist. \n";
}

sub MSG_E_3402 {
    my ($require_space, $current_space) = @_;
    print "\n$ERROR: [$MODULE-3402] Not enough space for operation (required $require_space KB, current $current_space KB). \n";
}
$MSG_E_3403 = "\n$ERROR: [$MODULE-3403] Intermediate syntax check failed!\n";
