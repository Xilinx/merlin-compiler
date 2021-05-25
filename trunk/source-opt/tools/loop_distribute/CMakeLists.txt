add_library(LoopDistribute
    "${CMAKE_CURRENT_SOURCE_DIR}/src/loop_distribute.cpp"
)

set_target_properties(LoopDistribute PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org LoopDistribute)
install(TARGETS LoopDistribute ARCHIVE DESTINATION lib)
