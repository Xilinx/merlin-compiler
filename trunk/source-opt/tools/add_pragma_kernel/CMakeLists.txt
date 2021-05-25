add_library(AddPragmaKernel
    "${CMAKE_CURRENT_SOURCE_DIR}/src/add_pragma_kernel.cpp"
)

set_target_properties(AddPragmaKernel PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org AddPragmaKernel)
install(TARGETS AddPragmaKernel ARCHIVE DESTINATION lib)
