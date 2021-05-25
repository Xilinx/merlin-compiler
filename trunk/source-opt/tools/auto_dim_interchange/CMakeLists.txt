add_library(AutoDimInterchange
    "${CMAKE_CURRENT_SOURCE_DIR}/src/auto_dim_interchange.cpp"
)

set_target_properties(AutoDimInterchange PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org AutoDimInterchange)
install(TARGETS AutoDimInterchange ARCHIVE DESTINATION lib)
