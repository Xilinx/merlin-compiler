function(compile_utils proj_name)
  add_executable(${proj_name}
      "${CMAKE_CURRENT_SOURCE_DIR}/${proj_name}/${proj_name}.cpp")
  
  target_link_libraries(${proj_name}
      legacy_ir
      legacy_parsers)
  
  set_target_properties(${proj_name} PROPERTIES
      LINKER_LANGUAGE CXX
      COMPILE_FLAGS "-DLINUX_COMMANDS")
endfunction(compile_utils)

compile_utils("cfg_gen")
compile_utils("cfg2def")
compile_utils("src_cfg")
compile_utils("set_cfg")
compile_utils("read")
