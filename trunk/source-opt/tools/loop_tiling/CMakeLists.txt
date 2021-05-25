add_library(LoopTiling
    "${CMAKE_CURRENT_SOURCE_DIR}/src/loop_tiling.cpp"
)

set_target_properties(LoopTiling PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org LoopTiling)
install(TARGETS LoopTiling ARCHIVE DESTINATION lib)
