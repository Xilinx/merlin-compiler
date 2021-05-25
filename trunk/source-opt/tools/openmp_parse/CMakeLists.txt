add_library(OpenmpParse
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openmp_parse.cpp"
)

set_target_properties(OpenmpParse PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org OpenmpParse)
install(TARGETS OpenmpParse ARCHIVE DESTINATION lib)
