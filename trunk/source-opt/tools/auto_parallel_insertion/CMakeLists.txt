add_library(AutoParallelInsertion
    "${CMAKE_CURRENT_SOURCE_DIR}/src/auto_parallel_insertion.cpp"
)

set_target_properties(AutoParallelInsertion PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org AutoParallelInsertion)
install(TARGETS AutoParallelInsertion ARCHIVE DESTINATION lib)

##unit_test
#link_directories(
#    "${ROSE_ROOT_DIR}/lib"
#    "${Boost_LIBRARY_DIRS}"
#    "${JNI_LIBRARY_DIRS}"
#    "${PROJECT_SOURCE_DIR}/flexlm")
#add_executable(auto_parallel_insertion_test
#    "${CMAKE_CURRENT_SOURCE_DIR}/src/auto_parallel_insertion_test.cpp")
#
#set_target_properties(auto_parallel_insertion_test PROPERTIES
#    LINKER_LANGUAGE CXX
#    COMPILE_FLAGS "-DLINUX_COMMANDS -DHAS_DS_MARK \
#    -DHAS_SYSTOLIC_FRONTEND -DHAS_SYSTOLIC_EXTRACT -DHAS_DS_LOOP_PARALLEL \
#    -DHAS_LOOP_PARALLEL -DHAS_MARS_IR \
#    -DHAS_STREAM_IR -DHAS_MEMORY_BURST -DHAS_LOWER_CPP \
#    -DHAS_DEPENDENCY_RESOLVE \
#    -DHAS_LOWER_SEPARATE -DHAS_COARSE_PIPELINE -DHAS_COARSE_PARALLEL \
#    -DHAS_DS_PIPELINE -DHAS_BITWIDTH_OPT \
#    -DHAS_DS_BITWIDTH_OPT -DHAS_REDUCTION -DHAS_REDUCTION_GENERAL \
#    -DHAS_PROGRAM_ANALYSIS -DHAS_ID_UPDATE -DHAS_PREPROCESS \
#    -DHAS_FUNCTION_INLINE -DHAS_AUTO_FUNC_INLINE \
#    -DHAS_AUTO_DIM_INTERCHANGE \
#    -DHAS_KERNEL_WRAPPER -DHAS_KERNEL_PRAGMA_GEN \
#    -DHAS_LOOP_TILING \
#    -DHAS_MEMORY_REDUCE -DHAS_MIDEND_PREPROCESS \
#    -DHAS_STRUCT_DECOMPOSE  -DHAS_COMM_OPT  -DHAS_ARRAY_LINEARIZE \
#    -DHAS_KERNEL_SEPARATE -DHAS_POSTWRAP_PROCESS -DHAS_MEMORY_PARTITION \
#    -DHAS_PERFORMANCE_ESTIMATION -DHAS_GEN_TOKEN_ID -DHAS_GEN_HIERARCHY \
#    -DHAS_ADD_PRAGMA_KERNEL -DHAS_FINAL_CODE_GEN -DHAS_DS_SCOPE -DHAS_DS_RESOURCE_EVAL -DHAS_DS_TILING \
#    -DHAS_DS_SKELETON_EXTRACT -DHAS_LINE_BUFFER -DHAS_CHANNEL_PARTITION ")
#
#target_link_libraries(auto_parallel_insertion_test 
#  gtest_main
#  AddPragmaKernel
#  ArrayDelinearize
#  ArrayLinearize
#  AutoFuncInline 
#  AutoParallelInsertion
#  BitwidthOpt
#  CoarsePipeline
#  CoarseParallel
#  CommOpt
#  ConstantPropagation
#  DataTransferGen
#  DetectNonStaticPointer
#  DetectPointerComparison
#  DetectPointerTypeCast
#  DetectMultiDimPointer
#  DsBitwidthOpt
#  DsLoopParallel
#  DsMark
#  DsPipeline
#  DsScope
#  DsResourceEval
#  DsSkeletonExtract
#  DsTiling
#  FinalCodeGen
#  FunctionInline
#  GenAccessRangeIntrisic  
#  GenHierarchy
#  GenTokenId
#  Ir
#  IdUpdate
#  InterfaceBoolConvert
#  InterfacePortRenaming
#  InterfaceTransform
#  AutoDimInterchange 
#  KernelPragmaGen
#  KernelSeparate
#  KernelWrapper
#  LoopFlatten
#  LoopParallel
#  LoopTiling
#  LowerCpp
#  LowerSeparate
#  MemoryBurst
#  MemoryPartition
#  ChannelPartition
#  MidendPreprocess
#  MemoryReduce
#  # MsgTest
#  DependencyResolve
#  StreamPrefetch
#  PerformanceEstimation
#  PostwrapProcess
#  Preprocess
#  ProgramAnalysis
#  Reduction
#  ReductionGeneral
#  RemoveSimplePointerAlias
#  ReplaceExpVisitor
#  SimpleDCE
#  StreamModel
#  StructDecompose
#  SystolicExtract
#  SystolicFrontend
#  LineBuffer
#  mars_rose
#  mars_opt_utils
#  legacy_ir
#  legacy_parsers
#  rose
#  ${Boost_LIBRARIES}
#  ${JNI_LIBRARIES}
#  gcrypt
#  ${CMAKE_DL_LIBS} 
#  ${CMAKE_THREAD_LIBS_INIT})
