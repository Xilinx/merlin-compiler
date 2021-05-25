add_library(DetectMultiDimPointer
    "${CMAKE_CURRENT_SOURCE_DIR}/src/detect_multidim_pointer.cpp"
)

set_target_properties(DetectMultiDimPointer PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org DetectMultiDimPointer)
install(TARGETS DetectMultiDimPointer ARCHIVE DESTINATION lib)
