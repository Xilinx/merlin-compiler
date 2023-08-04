
#!/bin/csh

set curr_prj=$1
set remote_dir=$2

if ($curr_prj == "") then 
    echo "Usage: $0 curr_prj remote_dir"
    exit;
endif
if (! -e "$curr_prj/run/") then 
    echo "Project 'run' is not under $curr_prj"
    exit;
endif
if ($remote_dir == "") then 
    echo "Usage: $0 curr_prj remote_dir"
    exit;
endif

if ($curr_prj == ".") then 
    set full_dir=`pwd`
    set prj_name=`basename $full_dir`
else
    set full_dir=$curr_prj
    set prj_name=`basename $full_dir`
endif

#set new_name=`basename $remote_dir`
#echo $new_name;

rm -rf /tmp/$prj_name
mkdir  /tmp/$prj_name/
scp -r /tmp/$prj_name/ $remote_dir

cp -r  $curr_prj /tmp/$prj_name
rm -rf /tmp/$prj_name/run/implement/exec/*

cd /tmp/$prj_name; tar czvf /tmp/$prj_name.tgz ./*; cd -
scp -r /tmp/$prj_name.tgz $remote_dir/
rm -rf /tmp/$prj_name/
rm -rf /tmp/$prj_name.tgz


