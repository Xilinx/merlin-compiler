add_library(DataStream
    "${CMAKE_CURRENT_SOURCE_DIR}/src/data_stream.cpp"
)

set_target_properties(DataStream PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org DataStream)
install(TARGETS DataStream ARCHIVE DESTINATION lib)
