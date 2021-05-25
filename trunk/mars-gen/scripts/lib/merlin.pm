package merlin;
use Exporter;

my $passwd = "falconisgreat";

our @ISA= qw( Exporter );

# these CAN be exported.
our @EXPORT_OK = qw(get_value_from_xml 
        print_error_msg 
        copy_src 
        query_exit 
        check_cpp 
        check_error 
        remove_space_in_pragma 
        str_shift_to_right 
        str_shift_to_left 
        read_file 
        write_file 
        replace_attribute 
        encrypt_dir
        decrypt_dir
        get_xocc_version
        remove_comment_scope
        get_aoc_version
        get_kernel_list
        get_kernel_file_list 
        get_task_list
        get_file_list);

# these are exported by default.
our @EXPORT = qw(get_value_from_xml 
        print_error_msg 
        copy_src 
        query_exit 
        check_cpp 
        check_error 
        remove_space_in_pragma 
        str_shift_to_right 
        str_shift_to_left 
        read_file 
        write_file 
        replace_attribute 
        encrypt_dir
        decrypt_dir
        get_xocc_version
        remove_comment_scope
        get_aoc_version
        get_kernel_list
        get_kernel_file_list 
        get_task_list
        get_file_list);

# get key value from xml file
sub get_value_from_xml {
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
    $bin_dir = "$MERLIN_COMPILER_HOME/build/bin";
    # stuff
    $key = $_[0];
    $xml = $_[1];
    $value = `$bin_dir/set_cfg -i get $xml $key`;
    chomp($value);
    #printf "key = $_[0], value = $value, xml = $_[1]\n";
    return $value;

}

# print_error_msg
sub print_error_msg {
    my ($msg,@addition) = @_;
    system "touch .cmost_flow_has_error";
    system "touch .merlin_flow_has_error";
    print "$msg";
    #if (not $is_silent)  { $input = <STDIN>; }
    die("\n");
}

# run command and print the command
#sub run_command {
#    if($xml_debug_mode eq "debug-level3") {
#        print "[" . [caller(0)]->[1] . ":" . [caller(0)]->[2] . "]";
#        print "# $_[0]\n";
#    }
#    system("$_[0]");
#}

# copy source file from one directory to another directory
sub copy_src {
    my $from = $_[0] ;
    my $to   = $_[1] ;
	#print "# copy_src $from $to \n";
    system "rm -rf $to/*";
    if (not -e "$to") { system "mkdir $to"; }
    system "cp -r $from/*.h   $to 2>/dev/null";
    system "cp -r $from/*.hpp $to 2>/dev/null";
    system "cp -r $from/*.c   $to 2>/dev/null";
    system "cp -r $from/*.cpp $to 2>/dev/null";
    system "cp -r $from/*.cl  $to 2>/dev/null";
    system "cp -r $from/*.xml $to 2>/dev/null";
    system "cp -r $from/kernel.list  $to 2>/dev/null";
    system "cp -r $from/aux   $to 2>/dev/null";
    system "cp -r $from/*.json $to 2>/dev/null";
    system "rm -rf $to/history*.json";
    system "cp -r $from/metadata $to/metadata 2>/dev/null";
    if(-e "$from/run_midend_preprocess") {
        system "cp -r $from/run_midend_preprocess $to/run_midend_preprocess 2>/dev/null";
    }
    if(-e "$from/src") {
        system "cp -r $from/src $to/src 2>/dev/null";
    }
}

# ask if flow go o nn
sub query_exit {
    my $msg         = $_[0] ;
    my $exit_option = $_[1] ;
    my $default     = $_[2] ;

    if ($default eq "yes") { $query = "[YES|no]"; }
    else                   { $query = "[yes|NO]"; }
    print "\n[merlin_flow] $msg $query ";
    $input = <STDIN>;
    chomp($input);
    $input = lc($input);

    $not_yes = ("yes" ne $input and "y" ne $input);
    $not_no  = ("no"  ne $input and "n" ne $input);
    $is_yes  = ("yes" eq $input or "y" eq $input);
    $is_no   = ("no"  eq $input or "n" eq $input);

#    print "\n input = +$input+ not_yes=${not_yes} is_yes=${is_yes} \n";
    if ($exit_option eq "yes" and $default eq "yes" and $not_no ) {system "touch .exit"; exit; }
    if ($exit_option eq "yes" and $default eq "no"  and $is_yes ) {system "touch .exit"; exit; }
    if ($exit_option eq "no"  and $default eq "yes" and $is_no  ) {system "touch .exit"; exit; }
    if ($exit_option eq "no"  and $default eq "no"  and $not_yes) {system "touch .exit"; exit; }
}

# check if directory have cpp file, except opencl_if.cpp openck_kernel_buffer.cpp and cmpst.cpp
sub check_cpp {
    my $file_dir = $_[0];
    $has_cpp = 0;
    $source_file_list = `ls $file_dir`;
    @source_file_set = split(/\n/, $source_file_list);
    foreach $one_file (@source_file_set) {
        if ($one_file =~ /(.*)\.cpp$/) {
            if($one_file eq "__merlin_opencl_if.cpp" or $one_file eq "__merlin_opencl_kernel_buffer.cpp" or $one_file eq "cmost.cpp") {
            } else {
                $has_cpp = 1;
            }
        }
    }
    return $has_cpp;
}

# check if directories have error
sub check_error
{
    if(-e ".merlin_flow_has_error" or
       -e ".cmost_flow_has_error") {
        exit;
#return 0;
    }
    for($i=0; $i<100; $i++) {
        if(-e "$_[$i]/.merlin_flow_has_error" or
           -e "$_[$i]/.cmost_flow_has_error") {
            system("touch .merlin_flow_has_error");
            exit;
#return 0;
        }
    }
#return 1;

#    my $report_dir_opt  = "report/opt";
#    if(-e $report_dir_opt) { system "rm -rf $report_dir_opt"; }
#    system "mkdir $report_dir_opt";
#    if(-e "merlin_opt.log") {system "cp merlin_opt.log $report_dir_opt";}

#    if (-e ".merlin_flow_has_error" or -e ".cmost_flow_has_error" ) {
#        if($developer_mode eq "user") {
#            system "rm -rf $src_code_transform >& /dev/null";
#            system "rm -rf $opencl_gen_dir >& /dev/null";
#        }
#        exit;
#    }
}

# remove all space in pragma in all files in specified directories
sub remove_space_in_pragma {
    $dir = $_[0];
    $file_list = `cd $dir/src; ls *.c* 2>/dev/null`;
    @file_set = split(/\n/, $file_list);
    foreach $one_file (@file_set) {
        system "cd $dir/src; perl -pi -e 's/\s*=\s*\"/=\"/g' $one_file";
        system "cd $dir/src; perl -pi -e 's/\s*=\s*\"/=\"/g' $one_file";
        system "cd $dir/src; perl -pi -e 's/\s*=\s*\"/=\"/g' $one_file";
        system "cd $dir/src; perl -pi -e 's/\s*=\s*\"/=\"/g' $one_file";
        system "cd $dir/src; perl -pi -e 's/\s*=\s*\"/=\"/g' $one_file";
    }
}

sub str_shift_to_left {
    $str_tmp = $_[0];
    $len_tmp = $_[1];
    if (@_ > 2)  {
        $max_len= $_[2];
    }

    my $i;

    $str_len = length($str_tmp);

    if ($str_len < $len_tmp) {
        for ($i = $str_len; $i < $len_tmp; $i+=1) {
            $str_tmp .= " ";
        }
    }
    if (@_ > 2 and $str_len > $len_tmp) {
        $str_tmp = substr($str_tmp, 0, $len_tmp);
    }
    return $str_tmp;
}
sub str_shift_to_right {
    $str_tmp = $_[0];
    $len_tmp = $_[1];
    if (@_ > 2)  {
        $max_len= $_[2];
    }

    my $i;

    $str_len = length($str_tmp);

    if ($str_len < $len_tmp) {
        for ($i = $str_len; $i < $len_tmp; $i+=1) {
            $str_tmp = " ".$str_tmp;
        }
    }
    if (@_ > 2 and $str_len > $len_tmp) {
        $str_tmp = substr($str_tmp, 0, $len_tmp);
    }
    return $str_tmp;
}

sub show_parents_process {
    my $pid = $$;
    while ( $pid > 0 ) {
        open my $stat_fh, "<", "/proc/$pid/stat"
            or die "no such pid: $pid\n";
        my $ppid = ( split /\s+/, <$stat_fh> )[3];
        close $stat_fh;
        print $pid, "\n";
        print `cat /proc/$pid/cmdline`, "\n";
        $pid = $ppid;
    }
}

sub read_file {
    my ($filename) = @_;

    open my $in, '<:encoding(UTF-8)', $filename or die "Could not open '$filename' for reading $!";
    local $/ = undef;
    my $all = <$in>;
    close $in;

    return $all;
}

sub write_file {
    my ($filename, $content) = @_;

    open my $out, '>:encoding(UTF-8)', $filename or die "Could not open '$filename' for writing $!";;
    print $out $content;
    close $out;

    return;
}

sub replace_attribute {
    my $xml = $_[0];
    my $key = $_[1];
    my $value = $_[2];
    my $extend = $_[3];
    my $temp = get_value_from_xml($key, $xml);
    my $data = read_file("$xml");
    my $xml_info = `cat $xml`;
    if($temp eq "") {
        $data =~ s/<directives>/<directives>\n<$key>$value<\/$key>/g;
    } else {
        if($extend eq 0) {
            $value = $value;
            $data =~ s/<\s*$key\s*>.*<\s*\/$key\s*>/<$key>$value<\/$key>/g;
        } elsif($extend eq 1) {
            if($xml_info =~ /<$key>(.*)<\/$key>/) {
                my $old = $1;
                $data =~ s/<\s*$key\s*>.*<\s*\/$key\s*>/<$key>$old $value<\/$key>/g;
#                $data =~ s/<directives>/<directives>\n<$key>$old $value<\/$key>/g;
            } else {
#                $value .= " $temp";
#                $data =~ s/<\s*$key\s*>.*<\s*\/$key\s*>/<$key>$value<\/$key>/g;
                $data =~ s/<directives>/<directives>\n<$key>$value<\/$key>/g;
            }
        }
    }
    write_file("$xml", $data);
}

sub remove_comment_scope {
    my $file_dir = $_[0];
    system "cd $file_dir; perl -pi -e 's/^\.\*__MERLIN_COMMENT_BEGIN__\.\*\$/\\\/\*/g' *.c* *.C 2>/dev/null";
    system "cd $file_dir; perl -pi -e 's/^\.\*__MERLIN_COMMENT_END__\.\*\$/\*\\\//g' *.c* *.C 2>/dev/null ";
}

sub encrypt_dir {
    my $file_dir = $_[0];
    my $info = `cd $file_dir; ls *.c* 2>/dev/null`;
    my @info_set = split(/\n/, $info);
    foreach my $one_line (@info_set) {
        system("cd $file_dir; script_enc $passwd $one_line");
    }
}

sub decrypt_dir {
    my $file_dir = $_[0];
    my $info = `cd $file_dir; ls *.c* 2>/dev/null`;
    my @info_set = split(/\n/, $info);
    foreach my $one_line (@info_set) {
        system("cd $file_dir; script_dec $passwd $one_line");
    }
}

sub get_xocc_version {
    my $version_str = `xocc --version`;
    if($version_str =~ /xocc v(\S+)(_sdx)?/) {
        return $1;
    }
    return '';
}

sub get_aoc_version {
    my $version_str = `aoc --version`;
    if ($version_str =~ /Version (\S+) Build/) {
        return $1;
    }
    return '';
}

sub get_kernel_list {
    my $file = $_[0];
    my $kernel_list = `cat $file`;
    my @kernel_list_array;
    my @list_set = split(/\n/, $kernel_list);
    foreach my $one_line (@list_set) {
    	if ($one_line =~ /\"(.*)\": \{/ ) {
            my $one_kernel_name = $1;
            push @kernel_list_array, $one_kernel_name;
        }
    }
    return @kernel_list_array;
}

sub get_file_list {
    my $file = $_[0];
    my $kernel_file = `cat $file`;
    my @kernel_file_array;
    my @file_set = split(/\n/, $kernel_file);
    foreach my $one_line (@file_set) {
    	if ($one_line =~ /\"(.*)\": \"(.*)\"/ ) {
            my $one_file_name = $2;
            if(! grep /$one_file_name/, @kernel_file_array) {
                push @kernel_file_array, $one_file_name;
            }
        }
    }
    return @kernel_file_array;
}

sub get_kernel_file_list {
    my $file = $_[0];
    my %kernel_list;
    my $kernel_name; 
    my $file_name;
    my @kernel_file_array;
    my $info = `cat $file`;
    my @list_set = split(/\n/, $info);
    foreach my $one_line (@list_set) {
		if ($one_line =~ /\"(.*)\"\s*: \{/ ) {
            $kernel_name = $1;
        }
		if ($one_line =~ /\"(.*)\"\s*: \"(.*)\"/ ) {
            $file_name = $2;
            if(! grep /$file_name/, @kernel_file_array) {
                $kernel_list{"$kernel_name"} .= " $file_name";
                push @kernel_file_array, $file_name;
            }
        }
    }
    return %kernel_list;
}

sub get_task_list {
    my $file = $_[0];
    my @task_list_array;
    if(-e "$file") {
        my $task_list = `cat $file`;
        my @list_set = split(/\n/, $task_list);
        foreach my $one_line (@list_set) {
        	if ($one_line =~ /\"(.*)\": \{/ ) {
                my $one_task_name = $1;
                push @task_list_array, $one_task_name;
            }
        }
    }
    return @task_list_array;
}
