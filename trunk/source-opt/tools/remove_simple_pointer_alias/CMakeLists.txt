add_library(RemoveSimplePointerAlias
    "${CMAKE_CURRENT_SOURCE_DIR}/src/remove_simple_pointer_alias.cpp"
)

set_target_properties(RemoveSimplePointerAlias PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org RemoveSimplePointerAlias)
install(TARGETS RemoveSimplePointerAlias ARCHIVE DESTINATION lib)
