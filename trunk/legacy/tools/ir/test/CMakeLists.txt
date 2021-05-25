add_executable(test_ir
    "${TOOLS_TOP}/ir/tldm_annotate.cpp"
    "${TOOLS_TOP}/ir/pltf_annotate.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_ir.cpp")

target_link_libraries(test_ir
    legacy_ir
    legacy_parsers
    m)

set_target_properties(test_ir PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")
