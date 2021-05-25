add_library(MemoryPartition
    "${CMAKE_CURRENT_SOURCE_DIR}/src/math_gcd.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/memory_partition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parallel_analysis.cpp"
)

set_target_properties(MemoryPartition PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org MemoryPartition)
install(TARGETS MemoryPartition ARCHIVE DESTINATION lib)
