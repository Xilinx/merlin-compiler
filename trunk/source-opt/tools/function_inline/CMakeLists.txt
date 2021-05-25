add_library(FunctionInline
    "${CMAKE_CURRENT_SOURCE_DIR}/src/function_inline.cpp"
)

set_target_properties(FunctionInline PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org FunctionInline)
install(TARGETS FunctionInline ARCHIVE DESTINATION lib)
