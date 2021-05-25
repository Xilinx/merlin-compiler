add_library(StreamModel
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_ir.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_ir_more.cpp"
)

set_target_properties(StreamModel PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org StreamModel)
install(TARGETS StreamModel ARCHIVE DESTINATION lib)
