$dist_dir = "dist";
$compiler_dir = $ENV{"MARS_COMPILER_HOME"};
$version = "2016.1";

sub run_command
{
	print "# $_[0]\n";
	system("$_[0]");
}

run_command "rm -rf $dist_dir";
run_command "mkdir  $dist_dir";

run_command "cp -r $compiler_dir/set_env            $dist_dir/set_env";
run_command "cp $compiler_dir/install_env.pl        $dist_dir";

run_command "cp -r $compiler_dir/legacy             $dist_dir";
run_command "rm -rf $dist_dir/legacy/xilinx_flow";
run_command "rm -rf $dist_dir/legacy/lib/*.tar.gz";
run_command "rm -rf $dist_dir/legacy/lib/boost_1_45_0";
run_command "rm -rf $dist_dir/legacy/lib/boost_1_45_0_gcc_4.8.2.patch";
run_command "rm -rf $dist_dir/legacy/lib/libstdc++6-4.6.1";
run_command "cp -r $compiler_dir/bin         	    $dist_dir";
#
run_command "cp -r $compiler_dir/apollo-vivado      $dist_dir";
run_command "cp -r $compiler_dir/self_test          $dist_dir";
#run_command "cp -r $compiler_dir/examples           $dist_dir";
run_command "cp -r $compiler_dir/VERSION            $dist_dir";
run_command "cp -r $compiler_dir/runtime            $dist_dir";

run_command "mkdir $dist_dir/source-opt";
run_command "cp -r $compiler_dir/source-opt/bin     $dist_dir/source-opt";
run_command "cp -r $compiler_dir/source-opt/lib     $dist_dir/source-opt";
run_command "cd $dist_dir/source-opt/lib; rm -rf *.tgz*";
run_command "cd $dist_dir/source-opt/lib; rm -rf *.tar*";
run_command "cd $dist_dir/source-opt/lib; rm -rf rose-0.9.6a-26323-jun2015";
run_command "cd $dist_dir/source-opt/lib; rm -rf rose-0.9.6a-26323-jun2015_r1_20160220";

run_command "mkdir $dist_dir/mars-gen";
run_command "cp -r $compiler_dir/mars-gen/bin       $dist_dir/mars-gen";
run_command "cp -r $compiler_dir/mars-gen/lib       $dist_dir/mars-gen";
run_command "cp -r $compiler_dir/mars-gen/default   $dist_dir/mars-gen";
run_command "cp -r $compiler_dir/mars-gen/scripts   $dist_dir/mars-gen";
run_command "cp -r $compiler_dir/mars-gen/test      $dist_dir/mars-gen";
run_command "cp -r $compiler_dir/mars-gen/drivers   $dist_dir/mars-gen";

run_command "mkdir $dist_dir/mars-gen/tools";
run_command "cp -r $compiler_dir/mars-gen/tools/add_header $dist_dir/mars-gen/tools";

# remove svn 
run_command "cd $dist_dir; perl $compiler_dir/legacy/scripts/merlin_flow/remove_svn.pl -s > /dev/null";

# remove temporary files
run_command "cd $dist_dir; find . -path '*' -name \"*.bak\" -exec rm -rf '{}' \\; > /dev/null";

# add license flags
run_command "rm -rf tmp_add_header";
run_command "cp -r $compiler_dir/mars-gen/tools/add_header/ tmp_add_header";
run_command "cd tmp_add_header; python addheader.py ../$dist_dir/legacy/scripts";
run_command "cd tmp_add_header; python addheader.py ../$dist_dir/legacy/scripts_1";
run_command "cd tmp_add_header; python addheader.py ../$dist_dir/legacy/system_ip";
run_command "cd tmp_add_header; python addheader.py ../$dist_dir/legacy/include/simple";

run_command "cd tmp_add_header; python addheader.py ../$dist_dir/mars-gen/scripts";
run_command "cd tmp_add_header; python addheader.py ../$dist_dir/mars-gen/test";
run_command "cd tmp_add_header; python addheader.py ../$dist_dir/mars-gen/drivers";

# encode the perl scripts
$key = "falconisgreat";
run_command "cd $dist_dir/legacy/scripts;   find . -path '*' -name \"*.pl\" -exec script_enc $key '{}' \\; > /dev/null";
run_command "cd $dist_dir/mars-gen/scripts; find . -path '*' -name \"*.pl\" -exec script_enc $key '{}' \\; > /dev/null";
run_command "cd $dist_dir/apollo-vivado;    find . -path '*' -name \"*.tcl\" -exec script_enc $key '{}' \\; > /dev/null";

# replace perl executor
run_command "cd $dist_dir/bin/; mv mars_perl_dec mars_perl;";


