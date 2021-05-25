add_library(AlteraPreprocess
    "${CMAKE_CURRENT_SOURCE_DIR}/src/altera_preprocess.cpp")

set_target_properties(AlteraPreprocess PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org AlteraPreprocess)
install(TARGETS AlteraPreprocess ARCHIVE DESTINATION lib)
