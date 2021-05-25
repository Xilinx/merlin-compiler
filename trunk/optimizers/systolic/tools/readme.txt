

Instructions to add a new pass to mars_opt
Please search and copy the content of "systolic_frontend", and replace "systolic_frontend" with your pass name

1. pass_name/
1.1  pass_name/makefile.inc
1.2  pass_name/Makefile
1.3  pass_name/src/*.cpp, *.h
1.4  pass_name/test/Makefile

2. mars_opt/
2.1 trunk/source-opt/tools/mars_opt/src/mars_opt.cpp
2.2 trunk/source-opt/tools/mars_opt/src/Makefile.am
2.2 trunk/source-opt/tools/mars_opt/configure.ac

3. build
3.1 trunk/Makefile //  add "export HAS_SYSTOLIC_FRONTEND = true" 
3.2 delete trunk/source-opt/tools/mars_opt/Makefile and rebuild

