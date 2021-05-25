add_library(InterfacePortRenaming
    "${CMAKE_CURRENT_SOURCE_DIR}/src/interface_port_renaming.cpp"
)

set_target_properties(InterfacePortRenaming PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org InterfacePortRenaming)
install(TARGETS InterfacePortRenaming ARCHIVE DESTINATION lib)
