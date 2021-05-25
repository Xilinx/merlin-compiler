add_library(LowerCpp
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lower_cpp.cpp"
)

set_target_properties(LowerCpp PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org LowerCpp)
install(TARGETS LowerCpp ARCHIVE DESTINATION lib)
