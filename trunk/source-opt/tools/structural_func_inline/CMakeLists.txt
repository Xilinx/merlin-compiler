add_library(StructuralFuncInline
    "${CMAKE_CURRENT_SOURCE_DIR}/src/structural_func_inline.cpp"
)

set_target_properties(StructuralFuncInline PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org StructuralFuncInline)
install(TARGETS StructuralFuncInline ARCHIVE DESTINATION lib)
