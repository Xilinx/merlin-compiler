add_library(InterfaceTransform
    "${CMAKE_CURRENT_SOURCE_DIR}/src/interface_transform.cpp"
)

set_target_properties(InterfaceTransform PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org InterfaceTransform)
install(TARGETS InterfaceTransform ARCHIVE DESTINATION lib)
