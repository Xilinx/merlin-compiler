add_library(StructDecompose
    "${CMAKE_CURRENT_SOURCE_DIR}/src/struct_decompose.cpp"
)

set_target_properties(StructDecompose PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org StructDecompose)
install(TARGETS StructDecompose ARCHIVE DESTINATION lib)
