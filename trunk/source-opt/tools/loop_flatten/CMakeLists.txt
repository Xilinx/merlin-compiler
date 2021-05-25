add_library(LoopFlatten
    "${CMAKE_CURRENT_SOURCE_DIR}/src/loop_flatten.cpp"
)

set_target_properties(LoopFlatten PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org LoopFlatten)
install(TARGETS LoopFlatten ARCHIVE DESTINATION lib)
