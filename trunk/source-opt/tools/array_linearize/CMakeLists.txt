add_library(ArrayLinearize
    "${CMAKE_CURRENT_SOURCE_DIR}/src/array_linearize.cpp"
)

set_target_properties(ArrayLinearize PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org ArrayLinearize)
install(TARGETS ArrayLinearize ARCHIVE DESTINATION lib)
