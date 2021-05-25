add_library(StreamPrefetch
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_prefetch.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_prefetch_xilinx.cpp"
)

set_target_properties(StreamPrefetch PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org StreamPrefetch)
install(TARGETS StreamPrefetch ARCHIVE DESTINATION lib)
