
eslopt_top = ${ESLOPT_HOME}
mars_top = ${MARS_HOME}
mars_build_dir = ${MARS_BUILD_DIR}
lib_dir = $(eslopt_top)/lib/

COMPONENT_INCLUDE = 
COMPONENT_SOURCE = 
COMPONENT_DEFINE = 

if HAS_MARS_IR
include ../../ir/makefile.inc
endif 

if HAS_STREAM_IR
include ../../stream_model/makefile.inc
endif 

if HAS_OPENMP_PARSE
include ../../openmp_parse/makefile.inc
endif 

if HAS_DATA_REUSE
include ../../data_reuse/makefile.inc
endif 

if HAS_LOOP_PARALLEL
include ../../loop_parallel/makefile.inc
endif 

if HAS_DS_LOOP_PARALLEL
include ../../ds_loop_parallel/makefile.inc
endif 

if HAS_DATA_STREAM
include ../../data_stream/makefile.inc
endif 

if HAS_DS_MEMORY_BURST
include ../../ds_memory_burst/makefile.inc
endif 

if HAS_MEMORY_BURST
include ../../memory_burst/makefile.inc
endif 

if HAS_LOWER_CPP
include ../../lower_cpp/makefile.inc
endif 

if HAS_LOWER_SEPARATE
include ../../lower_separate/makefile.inc
endif 

if HAS_COARSE_PIPELINE
include ../../coarse_pipeline/makefile.inc
endif 

if HAS_COARSE_PARALLEL
include ../../coarse_parallel/makefile.inc
endif 

if HAS_DS_PIPELINE
include ../../ds_pipeline/makefile.inc
endif 

if HAS_REDUCTION
include ../../reduction/makefile.inc
endif 

if HAS_REDUCTION_GENERAL
include ../../reduction_general/makefile.inc
endif 

if HAS_PROGRAM_ANALYSIS
include ../../program_analysis/makefile.inc
endif 

if HAS_ID_UPDATE
include ../../id_update/makefile.inc
endif 

if HAS_MSG_TEST
include ../../msg_test/makefile.inc
endif 

if HAS_BITWIDTH_OPT
include ../../bitwidth_opt/makefile.inc
endif 

if HAS_DS_BITWIDTH_OPT
include ../../ds_bitwidth_opt/makefile.inc
endif 

if HAS_PREPROCESS
include ../../preprocess/makefile.inc
endif

if HAS_FUNCTION_INLINE
include ../../function_inline/makefile.inc
endif 

if HAS_FUNCTION_OUTLINE
include ../../function_outline/makefile.inc
endif 

if HAS_KERNEL_WRAPPER
include ../../kernel_wrapper/makefile.inc
endif 

if HAS_KERNEL_PRAGMA_GEN
include ../../kernel_pragma_gen/makefile.inc
endif 

if HAS_OPENCL_TRANSFER_GEN
include ../../opencl_transfer_gen/makefile.inc
endif 

if HAS_LOOP_DISTRIBUTE
include ../../loop_distribute/makefile.inc
endif 

if HAS_LOOP_TILING
include ../../loop_tiling/makefile.inc
endif 

if HAS_ALTERA_PREPROCESS
include ../../altera_preprocess/makefile.inc
endif 

if HAS_COMM_OPT
include ../../comm_opt/makefile.inc
endif 

if HAS_ALTERA_POSTPROCESS
include ../../altera_postprocess/makefile.inc
endif 

if HAS_MEMORY_REDUCE
include ../../memory_reduce/makefile.inc
endif

if HAS_STRUCT_DECOMPOSE
include ../../struct_decompose/makefile.inc
endif

if HAS_ARRAY_LINEARIZE
include ../../array_linearize/makefile.inc
endif

if HAS_KERNEL_SEPARATE
include ../../kernel_separate/makefile.inc
endif

if HAS_ADD_PRAGMA_KERNEL
include ../../add_pragma_kernel/makefile.inc
endif

## ZP: FIXME: Headers!

if HAS_POSTWRAP_PROCESS
include ../../postwrap_process/makefile.inc
endif 

if HAS_SYSTOLIC_FRONTEND
include ../../../../optimizers/systolic/tools/systolic_frontend/makefile.inc
endif 

if HAS_SYSTOLIC_EXTRACT
include ../../../../optimizers/systolic/tools/systolic_extract/makefile.inc
endif 

if HAS_DESIGN_MODELING
include ../../../../optimizers/autodse/tools/design_modeling/makefile.inc
endif

if HAS_SEED_GENERATOR
include ../../../../optimizers/autodse/tools/seed_generator/makefile.inc
endif 

if HAS_MEMORY_PARTITION
include ../../memory_partition/makefile.inc
endif 

if HAS_CHANNEL_PARTITION
include ../../channel_partition/makefile.inc
endif 

if HAS_MIDEND_PREPROCESS
include ../../midend_preprocess/makefile.inc
endif

if HAS_PERFORMANCE_ESTIMATION
include ../../performance_estimation/makefile.inc
endif 

if HAS_GEN_TOKEN_ID
include ../../gen_token_id/makefile.inc
endif

if HAS_GEN_HIERARCHY
include ../../gen_hierarchy/makefile.inc
endif

if HAS_FINAL_CODE_GEN
include ../../final_code_gen/makefile.inc
endif
#############################################################################
SUBDIRS 		=

#############################################################################
MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		=						\
    -I$(eslopt_top)/tools/parsers/src \
    -I$(eslopt_top)/tools/ir \
    -I$(mars_top)/tools/rose \
    -I$(mars_top)/tools/ir/src \
    -I$(mars_build_dir)/lib/flexlm \
    -I$(mars_top)/lib/rapidjson \
    -I./ \
    $(COMPONENT_INCLUDE) \
	$(ROSE_CPPFLAGS) $(BOOST_CPPFLAGS)

#############################################################################

pkginclude_HEADERS 	=				

bin_PROGRAMS = mars_opt

mars_opt_SOURCES 	=			\
    $(COMPONENT_SOURCE) \
    $(eslopt_top)/tools/parsers/src/cmdline_parser.cpp \
    $(eslopt_top)/tools/parsers/src/file_parser.cpp \
    $(eslopt_top)/tools/parsers/src/xml_parser.cpp \
    $(mars_top)/tools/rose/codegen.cpp \
    $(mars_top)/tools/rose/program_analysis.cpp \
    $(mars_top)/tools/rose/common.cpp \
    $(mars_top)/tools/rose/locations.cpp \
    $(mars_top)/tools/rose/history.cpp \
    $(mars_top)/tools/rose/report.cpp \
    ./mars_opt.cpp \
    ./order_vector_gen.cpp \
    ./opencl_gen.cpp \
    ./prof_gen.cpp \
    ./dep_graph_gen.cpp \
    ./profiling.cpp \
    ./test_pass.cpp \
    $(eslopt_top)/tools/ir/tldm_annotate.cpp \
    $(eslopt_top)/tools/ir/pltf_annotate.cpp \
    $(eslopt_top)/tools/ir/PolyModel.cpp 

#    $(mars_top)/tools/$(COMPONENT)/src/$(COMPONENT).cpp 
    
FLMLIB = -llmgr_trl -lcrvs -lsb -lnoact -llmgr_dongle_stub
FLMDIR = ${mars_build_dir}/lib/flexlm

#PROFILE_FLAG = -pg
PROFILE_FLAG = 

mars_opt_CPPFLAGS	= \
	-gdwarf-3 \
	-fno-inline \
	$(PROFILE_FLAG) \
	-DLINUX_COMMANDS \
    $(COMPONENT_DEFINE) \
	$(ROSE_CPPFLAGS) $(BOOST_CPPFLAGS)

mars_opt_LDADD	=  \
	${FLMLIB} -L${mars_top}/lib -lpng12 -L/usr/lib64/ -L${FLMDIR} $(FLMDIR)/license.o $(FLMDIR)/lm_new.o \
	-ldl -lboost_filesystem -lboost_regex

#	$(lib_dir)/pocc/lib/libpocc.la \
#	$(lib_dir)/polyopt/lib/libpolyopt.la

mars_opt_LDFLAGS = -rdynamic \
				  $(PROFILE_FLAG) \
                  $(ROSE_LDFLAGS) $(BOOST_LDFLAGS)
 
## LNP: FIXME: Remove this: Rose is deprecated.
#CPPFLAGS += -Wno-deprecated -std=c++11
CPPFLAGS += -std=c++11 -Wall -pedantic

if PRODUCT_RELEASE
CPPFLAGS += -g -O2  
CFLAGS   += -g -O2  
CXXFLAGS += -g -O2  
else
CPPFLAGS += -Wall -pedantic -DPROJDEBUG=1 -g -O0 
CFLAGS   += -Wall -pedantic -DPROJDEBUG=1 -g -O0 
CXXFLAGS += -Wall -pedantic -DPROJDEBUG=1 -g -O0
endif


