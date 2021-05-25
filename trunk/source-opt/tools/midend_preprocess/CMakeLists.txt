add_library(MidendPreprocess
    "${CMAKE_CURRENT_SOURCE_DIR}/src/midend_preprocess.cpp"
)

set_target_properties(MidendPreprocess PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org MidendPreprocess)
install(TARGETS MidendPreprocess ARCHIVE DESTINATION lib)
