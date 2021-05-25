add_library(SynthesisCheck
    "${CMAKE_CURRENT_SOURCE_DIR}/src/synthesis_check.cpp"
)

set_target_properties(SynthesisCheck PROPERTIES
  LINKER_LANGUAGE CXX)
target_link_libraries(SynthesisCheck
  InputChecker)
add_dependencies(mars_opt_org SynthesisCheck)
install(TARGETS SynthesisCheck ARCHIVE DESTINATION lib)
