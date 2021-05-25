SHELL=/bin/bash -l

all: hook cmake_main
	@echo "Kind reminder for running \"make format\" before pull request"

hook:
	if [ ! -f "../.git/hooks/pre-commit" ]; then ln -s `git rev-parse --show-toplevel`/trunk/hooks/pre-commit ../.git/hooks/pre-commit; fi;

cmake_main:  
	cd build && cmake3 -GNinja -DCMAKE_BUILD_TYPE=Debug -DBOOST_ROOT=../source-opt/lib/boost_1_53_0 .. && ninja -v | \
		tee build.log && cat build.log | python ../filter_warnings.py | \
		tee warning.log

clean:
	git clean -dfx

check_python:
	which python

dse_check:
	cd build && ninja dse_check

format_dir=source-opt/tools  mars-gen/tools legacy/tools \
  optimizers/autodse/tools optimizers/systolic/tools/systolic_frontend/src \
  optimizers/systolic/tools/systolic_extract/src
format_auto_dir=$(addprefix trunk/, $(format_dir))
suffix_list=c cpp h
format_check=0

format:
	@echo "Start to format all the *.h, *.c and *.cpp"
	@echo "Remember to add all new files and directories to the repo"
	@d=$$(date +%s) && make format_core \
    && echo "Format took $$(($$(date +%s)-d)) seconds"
	@echo "Finish formating all the *.h, *.c and *.cpp"
format_core: 
	@rm -rf .format_file_list
	@touch .format_file_list
	@for dir in ${format_dir}; \
	  do \
	  find $${dir} -name *.c | cat >> .format_file_list; \
	  find $${dir} -name *.cpp | cat >> .format_file_list; \
	  find $${dir} -name *.h | cat >> .format_file_list ; \
	  done
	@format_count=0; for f in `cat .format_file_list`; \
	  do \
	  clang-format $$f >.format_output; \
	  if [ $$? -eq 0 ]; then \
	    diff --brief .format_output $$f &>/dev/null; \
	    if [ $$? -ne 0 ]; then \
	      mv .format_output $$f; \
		  let format_count=format_count+1; \
		  echo "Format $$f"; \
	    fi; \
	  fi; \
	  done; \
	echo "Format $${format_count} file(s)"; \
	if [ $${format_count} -ne 0 ] && [ ${format_check} -ne 0 ]; then \
	  echo "Need to run \"make format\" before pull request"; /bin/false; \
	fi
	@rm -rf .format_file_list
	@rm -rf .format_output

auto_format:
	@echo "Start to format all the *.h, *.c and *.cpp"
	@echo "Remember to add all new files and directories to the repo"
	@d=$$(date +%s) && make auto_format_core \
    && echo "Format took $$(($$(date +%s)-d)) seconds"
	@echo "Finish formating all the *.h, *.c and *.cpp"
auto_format_core: 
	@cd ..; \
	  rm -rf .format_file_list; \
	  touch .format_file_list; \
	  for dir in ${format_auto_dir}; \
	    do \
		  for sf in ${suffix_list}; \
		    do \
	          git status -uno $$dir |grep -e="*.$${sf}$$"|sed -En "s/#\tmodified:( +)//p" >> .format_file_list; \
	          git status -uno $$dir |grep -e="*.$${sf}$$"|sed -En "s/#\tnew file:( +)//p" >> .format_file_list; \
		    done;\
	    done; 
	@cd ..; \
	  format_count=0; cpplint_check_count=0; tidy_check_count=0; \
	  for f in `cat .format_file_list`; \
	    do \
		  echo $$f; \
	      clang-format $$f >.format_output; \
		  if [ $$? -eq 0 ]; then \
	        diff --brief .format_output $$f &>/dev/null; \
	        if [ $$? -ne 0 ]; then \
	          mv .format_output $$f; \
	          let format_count=format_count+1; \
	  	      echo "Format $$f"; \
	        fi; \
		  fi; \
	      cpplint --filter=-build/include_subdir,-legal/copyright $$f &> $$f.rpt; \
		  if [ `wc -l $$f.rpt | awk '{print $$1}'` -ne 1  ]; then \
	  	      echo "$$f breaks cpplint coding style requirement"; \
	          let cpplint_check_count=cpplint_check_count+1; \
		  fi; \
          /curr/software/dev-tools/llvm-8.0.1/bin/clang-tidy -p \
		  `pwd`/trunk/build -header-filter=.* $$f | sed '/merlin-deps/d' | sort | uniq > $$f.tidy; \
		  if [ `grep warning: $$f.tidy | wc -l | awk '{print $$1}'` -ne 0  ]; then \
	  	      echo "$$f breaks clang-tidy coding style requirement"; \
	          let tidy_check_count=tidy_check_count+1; \
		  fi; \
	    done; \
	  echo "Format $${format_count} file(s)"; \
	  echo "$${cpplint_check_count} file(s) failed in cpplint check"; \
	  echo "$${tidy_check_count} file(s) failed in clang-tidy check"; \
	  rm -rf .format_file_list; \
	  rm -rf .format_output; 
	@exit 0;
check:
	cd cpplint; export MERLIN_COMPILER_HOME=`pwd`/..; python3 run.py || echo "FAIL CPPLINT CHECKING";
	cd clang-tidy; export MERLIN_COMPILER_HOME=`pwd`/..; python3 run.py || echo "FAIL CLANG-TIDY CHECKING";
	echo "PASS ALL CHECKING"
