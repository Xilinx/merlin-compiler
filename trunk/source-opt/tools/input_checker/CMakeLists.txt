add_library(InputChecker
    "${CMAKE_CURRENT_SOURCE_DIR}/src/input_checker.cpp"
)

set_target_properties(InputChecker PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org InputChecker)
install(TARGETS InputChecker ARCHIVE DESTINATION lib)
