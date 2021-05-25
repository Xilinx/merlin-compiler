add_library(InterfaceBoolConvert
    "${CMAKE_CURRENT_SOURCE_DIR}/src/interface_convert_bool.cpp"
)

set_target_properties(InterfaceBoolConvert PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org InterfaceBoolConvert)
install(TARGETS InterfaceBoolConvert ARCHIVE DESTINATION lib)
