add_library(DetectNonStaticPointer
    "${CMAKE_CURRENT_SOURCE_DIR}/src/detect_non_static_pointer.cpp"
)

set_target_properties(DetectNonStaticPointer PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org DetectNonStaticPointer)
install(TARGETS DetectNonStaticPointer ARCHIVE DESTINATION lib)
