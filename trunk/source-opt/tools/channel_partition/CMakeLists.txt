add_library(ChannelPartition
    "${CMAKE_CURRENT_SOURCE_DIR}/src/channel_partition.cpp"
)

set_target_properties(ChannelPartition PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org ChannelPartition)
