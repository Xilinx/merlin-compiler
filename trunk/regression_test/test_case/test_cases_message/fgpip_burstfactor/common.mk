BASE_PATH:=$(MERLIN_COMPILER_HOME)/$(shell pwd | awk -F/  '{for(i=7;i<=NF;++i){printf "%s/",$$i};print "\n"}')
