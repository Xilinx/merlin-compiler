add_library(CoarsePipeline
    "${CMAKE_CURRENT_SOURCE_DIR}/src/coarse_pipeline.cpp"
)

set_target_properties(CoarsePipeline PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org CoarsePipeline)
install(TARGETS CoarsePipeline ARCHIVE DESTINATION lib)
