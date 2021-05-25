add_library(KernelSeparate
    "${CMAKE_CURRENT_SOURCE_DIR}/src/kernel_separate.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/pool_visit.cpp"
)

set_target_properties(KernelSeparate PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org KernelSeparate)
install(TARGETS KernelSeparate ARCHIVE DESTINATION lib)
