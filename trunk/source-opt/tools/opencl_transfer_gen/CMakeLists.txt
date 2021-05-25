add_library(OpenclTransferGen
    "${CMAKE_CURRENT_SOURCE_DIR}/src/opencl_transfer_gen.cpp"
)

set_target_properties(OpenclTransferGen PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org OpenclTransferGen)
install(TARGETS OpenclTransferGen ARCHIVE DESTINATION lib)
