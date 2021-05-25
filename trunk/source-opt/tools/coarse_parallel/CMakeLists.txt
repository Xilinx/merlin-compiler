add_library(CoarseParallel
    "${CMAKE_CURRENT_SOURCE_DIR}/src/coarse_parallel.cpp"
)

set_target_properties(CoarseParallel PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org CoarseParallel)
install(TARGETS CoarseParallel ARCHIVE DESTINATION lib)
