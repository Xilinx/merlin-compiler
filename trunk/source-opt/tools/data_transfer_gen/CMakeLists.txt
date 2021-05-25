add_library(DataTransferGen
    "${CMAKE_CURRENT_SOURCE_DIR}/src/data_transfer_gen.cpp"
)

set_target_properties(DataTransferGen PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org DataTransferGen)
install(TARGETS DataTransferGen ARCHIVE DESTINATION lib)
