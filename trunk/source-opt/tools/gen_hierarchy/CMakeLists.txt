add_library(GenHierarchy
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gen_hierarchy_top.cpp"
)

set_target_properties(GenHierarchy PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org GenHierarchy)
install(TARGETS GenHierarchy ARCHIVE DESTINATION lib)
