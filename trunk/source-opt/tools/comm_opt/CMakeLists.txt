add_library(CommOpt
    "${CMAKE_CURRENT_SOURCE_DIR}/src/channel_gen.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/channel_size.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/comm_check.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/comm_dead_remove.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/comm_opt.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/comm_refine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/comm_rename.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/comm_sync.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/comm_type.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/false_data_dep.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/reorder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/shared_mem_gen.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/coarse_pipeline_altera.cpp"
)

set_target_properties(CommOpt PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org CommOpt)
install(TARGETS CommOpt ARCHIVE DESTINATION lib)
