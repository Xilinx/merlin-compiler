add_library(Reduction
    "${CMAKE_CURRENT_SOURCE_DIR}/src/reduction.cpp"
)

set_target_properties(Reduction PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org Reduction)
install(TARGETS Reduction ARCHIVE DESTINATION lib)
