add_library(DetectLibraryCalls
    "${CMAKE_CURRENT_SOURCE_DIR}/src/detect_library_calls.cpp"
)

set_target_properties(DetectLibraryCalls PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org DetectLibraryCalls)
install(TARGETS DetectLibraryCalls ARCHIVE DESTINATION lib)
