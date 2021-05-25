add_library(DetectPointerTypeCast
    "${CMAKE_CURRENT_SOURCE_DIR}/src/detect_pointer_type_cast.cpp"
)

set_target_properties(DetectPointerTypeCast PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org DetectPointerTypeCast)
install(TARGETS DetectPointerTypeCast ARCHIVE DESTINATION lib)
