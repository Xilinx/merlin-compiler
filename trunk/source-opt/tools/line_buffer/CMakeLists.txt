add_library(LineBuffer
    "${CMAKE_CURRENT_SOURCE_DIR}/src/line_buffer.cpp"
)

set_target_properties(LineBuffer PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org LineBuffer)
install(TARGETS LineBuffer ARCHIVE DESTINATION lib)
