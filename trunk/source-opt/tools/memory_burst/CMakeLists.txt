add_library(MemoryBurst
    "${CMAKE_CURRENT_SOURCE_DIR}/src/memory_burst.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/memory_burst_cg.cpp"
)

set_target_properties(MemoryBurst PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org MemoryBurst)
install(TARGETS MemoryBurst ARCHIVE DESTINATION lib)
