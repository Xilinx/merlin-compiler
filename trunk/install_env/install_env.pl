
$tool = $ARGV[0];
$runtime = $ARGV[1];

if (not -e "./install_env.pl") {
    print "[install_env] ERROR: this script must be called at installation directory\n";
    exit;
}

$pwd = `pwd`;
chomp($pwd);

sub check_xilinx {
    print("\nChecking SDX ... \n" );
    $sda1= `sdaccel -version`;
    chomp($sda1);
    $sda2 = `sdx -version`;
    chomp($sda2);
    if ($sda1 !~ /Xilinx/ and $sda2 !~ /Xilinx/) {
        print "Xilinx SDX is not found.\nPlease source SDX setting64.sh or setting64.csh first!\n";
	    exit;
    }
    if ($sda1 =~ /Xilinx/) {
        $sdaccel_flag = 1;
        $sda= `which sdaccel`;
        chomp($sda);
        if ($sda =~ /(.*)\/bin\/sda/) {
            $sda_path = $1;
        }
    }
    if ($sda2 =~ /Xilinx/) {
        $sdx_flag = 1;
        $sda= `which xocc`;
        chomp($sda);
        if ($sda =~ /(.*)\/bin\/xocc/) {
            $sda_path = $1;
        }
    }
}

if($tool eq "xilinx") {
    check_xilinx;
} elsif($tool eq "Xilinx") {
    $tool = "xilinx";
    check_xilinx;
} elsif($tool eq "falcon") {
    printf "Falcon tool not open now\n";
    exit;
} elsif($tool eq "all") {
    check_xilinx;
} else {
    printf "Please specify correct tool : xilinx / all!\n";
    printf "    Example: perl install_env.pl all\n";
    exit;
}

if($runtime eq "kestrel") {
} else {
}

#set csh environment
system "echo 'setenv MERLIN_COMPILER_HOME $pwd' 				                >  settings64.csh";
system "echo 'echo \"MERLIN_COMPILER_HOME = \$MERLIN_COMPILER_HOME\"' 	        >> settings64.csh";
system "echo 'setenv PATH \$MERLIN_COMPILER_HOME/bin:\$PATH' 		            >> settings64.csh";

if($tool eq "xilinx" or $tool eq "all") {
    system "echo 'setenv XILINX_SDX $sda_path' 			                    >> settings64.csh";
    system "echo 'echo \"XILINX_SDX = \$XILINX_SDX\"' 	                >> settings64.csh";
}

#set bash environment
system "echo 'export MERLIN_COMPILER_HOME=$pwd' 			                    >  settings64.sh";
system "echo 'echo \"MERLIN_COMPILER_HOME = \$MERLIN_COMPILER_HOME\"' 	        >> settings64.sh";
system "echo 'export PATH=\$MERLIN_COMPILER_HOME/bin:\$PATH' 		            >> settings64.sh";

if($tool eq "xilinx" or $tool eq "all") {
    system "echo 'export XILINX_SDX=$sda_path' 			                    >> settings64.sh";
    system "echo 'echo \"XILINX_SDX = \$XILINX_SDX\"' 	                >> settings64.sh";
}

if (-e "settings64.csh") {
    print "Settings scripts settings64.csh and settings64.sh are generated successfully.\n";
} else {
    print "Failed to generate settings64.csh and settings64.sh.\n";
}
