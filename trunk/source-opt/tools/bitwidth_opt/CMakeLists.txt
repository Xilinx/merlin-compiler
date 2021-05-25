add_library(BitwidthOpt
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bitwidth_opt.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/memory_coalescing_intel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/memory_coalescing_xilinx.cpp"
)

set_target_properties(BitwidthOpt PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org BitwidthOpt)
install(TARGETS BitwidthOpt ARCHIVE DESTINATION lib)
