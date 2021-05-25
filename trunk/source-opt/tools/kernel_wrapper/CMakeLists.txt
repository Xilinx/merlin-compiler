add_library(KernelWrapper
    "${CMAKE_CURRENT_SOURCE_DIR}/src/kernel_wrapper.cpp"
)

set_target_properties(KernelWrapper PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org KernelWrapper)
install(TARGETS KernelWrapper ARCHIVE DESTINATION lib)
