add_library(GenAccessRangeIntrisic
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gen_access_range_intrinsic.cpp"
)

set_target_properties(GenAccessRangeIntrisic PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org GenAccessRangeIntrisic)
install(TARGETS GenAccessRangeIntrisic ARCHIVE DESTINATION lib)
