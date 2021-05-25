add_library(Preprocess
    "${CMAKE_CURRENT_SOURCE_DIR}/src/preprocess.cpp"
)

set_target_properties(Preprocess PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org Preprocess)
install(TARGETS Preprocess ARCHIVE DESTINATION lib)
