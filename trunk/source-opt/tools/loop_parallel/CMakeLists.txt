add_library(LoopParallel
    "${CMAKE_CURRENT_SOURCE_DIR}/src/loop_parallel.cpp"
)

set_target_properties(LoopParallel PROPERTIES
  LINKER_LANGUAGE CXX)

# add this because loop_flatten_for_pipeline function 
# was used in loop_parallel
target_link_libraries(LoopParallel LoopFlatten)
add_dependencies(mars_opt_org LoopParallel)
install(TARGETS LoopParallel ARCHIVE DESTINATION lib)
