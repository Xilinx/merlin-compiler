add_library(ReplaceExpVisitor
    "${CMAKE_CURRENT_SOURCE_DIR}/src/replace_exp_visitor.cpp"
)

set_target_properties(ReplaceExpVisitor PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org ReplaceExpVisitor)
install(TARGETS ReplaceExpVisitor ARCHIVE DESTINATION lib)
