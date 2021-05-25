add_library(DataReuse
    "${CMAKE_CURRENT_SOURCE_DIR}/src/data_reuse.cpp"
)

set_target_properties(DataReuse PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org DataReuse)
install(TARGETS DataReuse ARCHIVE DESTINATION lib)
