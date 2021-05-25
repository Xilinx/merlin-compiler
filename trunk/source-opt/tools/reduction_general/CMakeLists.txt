add_library(ReductionGeneral
    "${CMAKE_CURRENT_SOURCE_DIR}/src/reduction_general.cpp"
)

set_target_properties(ReductionGeneral PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org ReductionGeneral)
install(TARGETS ReductionGeneral ARCHIVE DESTINATION lib)
