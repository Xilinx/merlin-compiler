add_library(FunctionOutline
    "${CMAKE_CURRENT_SOURCE_DIR}/src/function_outline.cpp"
)

set_target_properties(FunctionOutline PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org FunctionOutline)
install(TARGETS FunctionOutline ARCHIVE DESTINATION lib)
