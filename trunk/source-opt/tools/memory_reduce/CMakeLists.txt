add_library(MemoryReduce
    "${CMAKE_CURRENT_SOURCE_DIR}/src/memory_reduce.cpp"
)

set_target_properties(MemoryReduce PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org MemoryReduce)
install(TARGETS MemoryReduce ARCHIVE DESTINATION lib)
