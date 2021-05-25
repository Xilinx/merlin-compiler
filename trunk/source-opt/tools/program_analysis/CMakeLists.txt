add_library(ProgramAnalysis
    "${CMAKE_CURRENT_SOURCE_DIR}/src/program_analysis_test.cpp"
)

set_target_properties(ProgramAnalysis PROPERTIES
  LINKER_LANGUAGE CXX)

# add this because loop_flatten_for_pipeline function 
# was used in loop_parallel
target_link_libraries(ProgramAnalysis LoopFlatten)
add_dependencies(mars_opt_org ProgramAnalysis)
install(TARGETS ProgramAnalysis ARCHIVE DESTINATION lib)
