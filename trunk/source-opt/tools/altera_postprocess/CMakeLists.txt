add_library(AlteraPostprocess
    "${CMAKE_CURRENT_SOURCE_DIR}/src/altera_postprocess.cpp"
)

set_target_properties(AlteraPostprocess PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org AlteraPostprocess)
install(TARGETS AlteraPostprocess ARCHIVE DESTINATION lib)
