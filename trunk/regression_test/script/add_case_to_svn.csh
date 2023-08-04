#!/bin/csh

set num=$1
set prj_dir=$2

if (-e ~/TEST_ENV/bugzilla/$num) then 
  echo "Case directory already exists: ~/TEST_ENV/bugzilla/$num"
  exit
endif

mkdir ~/TEST_ENV/bugzilla/$num
cp -r src ~/TEST_ENV/bugzilla/$num

if ($prj_dir != "") then
   if (! -e $prj_dir) then
    echo "The build directory $prj_dir does not exists"
    exit
   endif

   cp -r $prj_dir ~/TEST_ENV/bugzilla/$num; 
   if (-e include/) then
       cp -r include/ ~/TEST_ENV/bugzilla/$num; 
   endif
   rm -rf ~/TEST_ENV/bugzilla/$num/$prj_dir/implement* >& /dev/null
   rm -rf ~/TEST_ENV/bugzilla/$num/$prj_dir/pkg* >& /dev/null
   rm -rf ~/TEST_ENV/bugzilla/$num/build/.merlin_prj >& /dev/null
   if (-e ~/TEST_ENV/bugzilla/$num/build) then
        make -C ~/TEST_ENV/bugzilla/$num/build clean >& /dev/null
   endif 
endif

cd ~/TEST_ENV/bugzilla/
svn add $num; 
svn ci -m "" $num;
cd -

echo "Case #$num is uploaded successfully."


