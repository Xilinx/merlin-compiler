add_library(KernelPragmaGen
    "${CMAKE_CURRENT_SOURCE_DIR}/src/kernel_pragma_gen.cpp"
)

set_target_properties(KernelPragmaGen PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org KernelPragmaGen)
install(TARGETS KernelPragmaGen ARCHIVE DESTINATION lib)
