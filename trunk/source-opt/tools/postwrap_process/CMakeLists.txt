add_library(PostwrapProcess
    "${CMAKE_CURRENT_SOURCE_DIR}/src/postwrap_process.cpp"
)

set_target_properties(PostwrapProcess PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org PostwrapProcess)
install(TARGETS PostwrapProcess ARCHIVE DESTINATION lib)
