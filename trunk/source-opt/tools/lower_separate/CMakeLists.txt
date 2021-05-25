add_library(LowerSeparate
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lower_separate.cpp")

set_target_properties(LowerSeparate PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org LowerSeparate)
install(TARGETS LowerSeparate ARCHIVE DESTINATION lib)
