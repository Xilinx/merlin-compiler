add_library(DetectPointerComparison
    "${CMAKE_CURRENT_SOURCE_DIR}/src/detect_pointer_compare.cpp"
)

set_target_properties(DetectPointerComparison PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org DetectPointerComparison)
install(TARGETS DetectPointerComparison ARCHIVE DESTINATION lib)
