add_library(Ir
    "${CMAKE_CURRENT_SOURCE_DIR}/src/analPragmas.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bsuGroup.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bsuNode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bsuNode_v2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mars_ir.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mars_ir_v2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mars_ir_test.cpp"
)

set_target_properties(Ir PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org Ir)
install(TARGETS Ir ARCHIVE DESTINATION lib)
