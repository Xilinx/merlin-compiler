#include_directories("${PROJECT_SOURCE_DIR}/flexlm")

link_directories(
    "${ROSE_ROOT_DIR}/lib"
    "${Boost_LIBRARY_DIRS}"
    "${JNI_LIBRARY_DIRS}")
#    "${PROJECT_SOURCE_DIR}/flexlm")

add_library(mars_opt_utils STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mars_opt_utils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mars_preproc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/order_vector_gen.cpp")


file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h)
set_target_properties(mars_opt_utils PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

install(TARGETS mars_opt_utils
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/merlin)

add_executable(mars_opt_org
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mars_opt.cpp")

set_target_properties(mars_opt_org PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS -DHAS_DS_MARK \
    -DHAS_DS_LOOP_PARALLEL \
    -DHAS_LOOP_PARALLEL -DHAS_MARS_IR \
    -DHAS_STREAM_IR -DHAS_MEMORY_BURST -DHAS_LOWER_CPP \
    -DHAS_DEPENDENCY_RESOLVE \
    -DHAS_LOWER_SEPARATE -DHAS_COARSE_PIPELINE -DHAS_COARSE_PARALLEL \
    -DHAS_DS_PIPELINE -DHAS_BITWIDTH_OPT \
    -DHAS_DS_BITWIDTH_OPT -DHAS_REDUCTION -DHAS_REDUCTION_GENERAL \
    -DHAS_PROGRAM_ANALYSIS -DHAS_ID_UPDATE -DHAS_PREPROCESS \
    -DHAS_FUNCTION_INLINE -DHAS_AUTO_FUNC_INLINE \
    -DHAS_AUTO_DIM_INTERCHANGE \
    -DHAS_KERNEL_WRAPPER -DHAS_KERNEL_PRAGMA_GEN \
    -DHAS_LOOP_TILING -DHAS_ALTERA_PREPROCESS \
    -DHAS_MIDEND_PREPROCESS -DHAS_ALTERA_POSTPROCESS \
    -DHAS_STRUCT_DECOMPOSE  -DHAS_COMM_OPT  -DHAS_ARRAY_LINEARIZE \
    -DHAS_KERNEL_SEPARATE -DHAS_POSTWRAP_PROCESS -DHAS_MEMORY_PARTITION \
    -DHAS_PERFORMANCE_ESTIMATION -DHAS_GEN_TOKEN_ID -DHAS_GEN_HIERARCHY \
    -DHAS_ADD_PRAGMA_KERNEL -DHAS_FINAL_CODE_GEN -DHAS_DS_SCOPE -DHAS_DS_RESOURCE_EVAL -DHAS_DS_TILING \
    -DHAS_DS_SKELETON_EXTRACT -DHAS_LINE_BUFFER ")

target_link_libraries(mars_opt_org
  AddPragmaKernel
  ArrayDelinearize
  ArrayLinearize
  AutoFuncInline 
  AutoParallelInsertion
  BitwidthOpt
  CoarsePipeline
  CoarseParallel
  CommOpt
  ConstantPropagation
  DataTransferGen
  DetectNonStaticPointer
  DetectPointerComparison
  DetectPointerTypeCast
  DetectMultiDimPointer
  DetectLibraryCalls
  DsBitwidthOpt
  DsLoopParallel
  DsMark
  DsPipeline
  DsScope
  DsResourceEval
  DsSkeletonExtract
  DsTiling
  FinalCodeGen
  FunctionInline
  GenAccessRangeIntrisic  
  GenHierarchy
  GenTokenId
  Ir
  IdUpdate
  InputChecker
  InterfaceBoolConvert
  InterfacePortRenaming
  InterfaceTransform
  AutoDimInterchange 
  KernelPragmaGen
  KernelSeparate
  KernelWrapper
  LoopFlatten
  LoopParallel
  LoopTiling
  LowerCpp
  LowerSeparate
  MemoryBurst
  MemoryPartition
  MidendPreprocess
  DependencyResolve
  StreamPrefetch
  PerformanceEstimation
  PostwrapProcess
  Preprocess
  ProgramAnalysis
  Reduction
  ReductionGeneral
  RemoveSimplePointerAlias
  ReplaceExpVisitor
  SimpleDCE
  StreamModel
  StructDecompose
  StructuralFuncInline
  SynthesisCheck
  LineBuffer
  mars_rose
  mars_opt_utils
  legacy_ir
  legacy_parsers
  rose
  ${Boost_LIBRARIES}
  quadmath
  ${CMAKE_DL_LIBS} 
  ${CMAKE_THREAD_LIBS_INIT})
