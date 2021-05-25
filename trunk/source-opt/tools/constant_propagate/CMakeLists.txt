add_library(ConstantPropagation
    "${CMAKE_CURRENT_SOURCE_DIR}/src/constant_propagate.cpp"
)

set_target_properties(ConstantPropagation PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org ConstantPropagation)
install(TARGETS ConstantPropagation ARCHIVE DESTINATION lib)
