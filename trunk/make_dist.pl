
#---------------------------------------------------------------------------------------------#
# excute before everything
#---------------------------------------------------------------------------------------------#
my $curr_dir;
my $MERLIN_COMPILER_HOME;
my $library_path;
BEGIN {
    $|=1;  #turn on auto reflush for stdout
    $curr_dir = `pwd`;
    chomp($curr_dir);
    $ENV{"MERLIN_CURR_PROJECT"} = $curr_dir;
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
}

#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use lib "$library_path";
use merlin;

$dist_dir = "Merlin_Compiler";
$compiler_dir = $ENV{"MERLIN_COMPILER_HOME"};

#########################################################
# ZP: force to set the version number when creating release build
# NOTE: the release number is required to provide from QA team
#my $arg_num = @ARGV;
#
#if ($arg_num < 1) {
#  print "Error: a release number is required, e.g. 1.0.2\n";
#  print "Usage: \n";
#  print "  perl \$MERLIN_COMPILER_HOME/make_dist.pl version_number\n";
#  exit();
#}
#
#if ($arg_num > 1) {
#  print "Error: Too many argument\n";
#  print "Usage: \n";
#  print "  perl \$MERLIN_COMPILER_HOME/make_dist.pl version_number\n";
#  exit();
#}

#my $release_number = $ARGV[0];
#
#if ($release_number !~ /^\d+\.\d+\.\d+/) {
#  print "Error: Invalid format for release number\n";
#  print "Valid format: e.g.\n";
#  print "  2.0.1 \n";
#  print "  3.0.1_rc1 \n";
#  print "  4.2.3_customer1 \n";
#  exit();
#}
#
## give warning if version check is not passed
#if ($release_number =~ /^(\d+)\.(\d+)\.\d+/) {
#  my $ver_input_0 = $1;
#  my $ver_input_1 = $2;
#
#  my $year_num = `date +%Y`;
#  my $ver_num_0 = $year_num - 2016;
#
#  my $quarter_num_t = `date +%m`;
#  my $ver_num_1= ( int(($quarter_num_t-1) / 3) );
#  if ($ver_num_0 == 1) {
#    $ver_num_1 = $ver_num_1 - 2;
#  }
#  if ($ver_input_0 != $ver_num_0 or $ver_input_1 != $ver_num_1) {
#    print "\nWarning: According to the release date, the suggestion version prefix is ${ver_num_0}.${ver_num_1}. \n";
#    query_exit("Are you sure to use the input version number anyway?", "no", "no");
#  }
#}

#if ('add_version' ~~ @ARGV) {
#    increase_build_version();
#} else {
    do_build();
#}
exit();

sub run_command {
	print "# $_[0]\n";
	system("$_[0]");
}

sub do_build {
    run_command "rm -rf $dist_dir";
    run_command "mkdir  $dist_dir";

    run_command "cp $compiler_dir/build/.version        	$dist_dir/.version";

###############################################################
#ZP: 20170903
#    run_command "echo \"$release_number\"   > $dist_dir/.rversion";
###############################################################
    if (1) {
    run_command "cp -r $compiler_dir/set_env            			$dist_dir/set_env";
    run_command "cp $compiler_dir/install_env/install_env.pl        	$dist_dir/install_env.pl";



    run_command "mkdir $dist_dir/legacy";
    run_command "cp -r $compiler_dir/legacy/scripts         $dist_dir/legacy/";

    run_command "cp -r $compiler_dir/bin         	    			$dist_dir";
    run_command "mkdir                           	    			$dist_dir/build";
    run_command "cp -r $compiler_dir/build/bin         	    $dist_dir/build";
    run_command "cp -r $compiler_dir/build/.version         $dist_dir/build";

    run_command "mkdir $dist_dir/source-opt";
    run_command "cp -r $compiler_dir/source-opt/lib     			$dist_dir/source-opt";
    run_command "cd $dist_dir/source-opt/lib; rm -rf *.tgz*";
    run_command "cd $dist_dir/source-opt/lib; rm -rf *.tar*";
    run_command "cd $dist_dir/source-opt/lib; rm -rf .extract*";

    run_command "mkdir $dist_dir/source-opt/include";
    run_command "cp -r $compiler_dir/source-opt/include/apint_include   $dist_dir/source-opt/include";

    run_command "mkdir $dist_dir/mars-gen";

    run_command "cp -r $compiler_dir/mars-gen/lib       			        $dist_dir/mars-gen";
    run_command "rm -rf $dist_dir/mars-gen/lib/flexlm/license.*";

    run_command "cp -r $compiler_dir/mars-gen/default   			        $dist_dir/mars-gen";
    run_command "cp -r $compiler_dir/mars-gen/scripts   			        $dist_dir/mars-gen";
    run_command "mkdir                                  			        $dist_dir/mars-gen/drivers";
    run_command "cp -r $compiler_dir/mars-gen/drivers/opencl          $dist_dir/mars-gen/drivers";
    run_command "cp -r $compiler_dir/mars-gen/drivers/code_transform  $dist_dir/mars-gen/drivers";
    run_command "cp -r $compiler_dir/mars-gen/drivers/cmost_headers   $dist_dir/mars-gen/drivers";

    run_command "mkdir $dist_dir/mars-gen/tools";
    run_command "cp -r $compiler_dir/mars-gen/tools/add_header 		$dist_dir/mars-gen/tools";
    run_command "cp -r $compiler_dir/mars-gen/tools/llvm          $dist_dir/mars-gen/tools";

    run_command "mkdir $dist_dir/optimizers";
# autodse
    run_command "mkdir $dist_dir/optimizers/autodse";
    run_command "cp -r $compiler_dir/optimizers/autodse/configs   $dist_dir/optimizers/autodse/";
    run_command "cp -r $compiler_dir/optimizers/autodse/scripts   $dist_dir/optimizers/autodse/";
    run_command "cp -r $compiler_dir/optimizers/autodse/libs      $dist_dir/optimizers/autodse/";
    run_command "rm -rf $dist_dir/optimizers/autodse/libs/{MerlinPyEnv,python*}";
    run_command "cd    $dist_dir/optimizers/autodse/scripts/; find . -path '*' -name \"*.py\" -exec rm -rf '{}' \\; > /dev/null; cd -";
# systolic array
    run_command "mkdir $dist_dir/optimizers/systolic";
    run_command "cp -r $compiler_dir/optimizers/systolic/tools   $dist_dir/optimizers/systolic/";
    run_command "cp -r $compiler_dir/optimizers/systolic/templates_v0_general   $dist_dir/optimizers/systolic/";
    run_command "cp -r $compiler_dir/optimizers/systolic/scripts      $dist_dir/optimizers/systolic/";
    #run_command "cp -r $compiler_dir/optimizers/systolic/examples      $dist_dir/optimizers/systolic/";

# directive xml file
    run_command "cp $compiler_dir/mars-gen/default/release_directive_mars_basic.xml $dist_dir/mars-gen/default/directive_mars_basic.xml";
    run_command "cp $compiler_dir/mars-gen/default/release_directive_mars_basic_altera.xml $dist_dir/mars-gen/default/directive_mars_basic_altera.xml";

#delete files
    run_command "rm -rf $dist_dir/runtime";

## remove svn
#    run_command "cd $dist_dir; perl $compiler_dir/legacy/scripts/merlin_flow/remove_svn.pl -s > /dev/null";

# remove temporary files
    run_command "cd $dist_dir; find . -path '*' -name \"*.bak\" -exec rm -rf '{}' \\; > /dev/null";

# add license flags
    run_command "rm -rf tmp_add_header";
    run_command "cp -r $compiler_dir/mars-gen/tools/add_header/ tmp_add_header";

    run_command "cd tmp_add_header; python addheader.py ../$dist_dir/mars-gen/scripts";
    run_command "cd tmp_add_header; python addheader.py ../$dist_dir/mars-gen/test";
    run_command "cd tmp_add_header; python addheader.py ../$dist_dir/mars-gen/drivers";

# encode the perl scripts
    $key = "falconisgreat";
    $enc_dir = "$dist_dir/mars-gen/scripts";
    run_command "cd $enc_dir; find . -path '*' -name \"*.pl\" -exec script_enc $key '{}' \\; >& /dev/null";
    $enc_dir = "$dist_dir/mars-gen/drivers/code_transform/coalescing";
    run_command "cd $enc_dir; find . -path '*' -name \"*.*\" -exec script_enc $key '{}' \\; >& /dev/null";
    $enc_dir = "$dist_dir/optimizers/systolic/scripts";
    run_command "cd $enc_dir; find . -path '*' -name \"*.pl\" -exec script_enc $key '{}' \\; >& /dev/null";

# replace perl executor
    run_command "cd $dist_dir/build/bin/; mv mars_perl_dec mars_perl;";
    run_command "cd $dist_dir/build/bin/; mv mars_copy_dec mars_copy;";

    run_command "cp -r /curr/mars/scripts/dist_file/* $dist_dir/bin";
}

    print "******************************************************\n";
    print "Merlin Compiler release generated successfully \n";
    print "******************************************************\n";
    print "Please verify the following version information:  \n";
    system "cat $dist_dir/.version";
#    print "Merlin release version: ";
#    system "cat $dist_dir/.rversion";

#    print "\n";
#    print "The following is to verify the version info in the installation: \n";
#    print "    > cd $dist_dir/; perl install_env.pl \n";
#    print "    > source $dist_dir/setttings64.sh \n";
#    print "    > merlincc -v \n";
#    print "\n";

}
