add_library(AutoFuncInline
    "${CMAKE_CURRENT_SOURCE_DIR}/src/auto_func_inline.cpp"
)

set_target_properties(AutoFuncInline PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org AutoFuncInline)
install(TARGETS AutoFuncInline ARCHIVE DESTINATION lib)
