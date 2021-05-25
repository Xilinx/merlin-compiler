add_library(SimpleDCE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/simple_dce.cpp"
)

set_target_properties(SimpleDCE PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org SimpleDCE)
install(TARGETS SimpleDCE ARCHIVE DESTINATION lib)
