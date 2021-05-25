find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

include_directories("${TOOLS_TOP}/parsers/src"
                    "${CMAKE_CURRENT_SOURCE_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}")

bison_target(Bisx "${CMAKE_CURRENT_SOURCE_DIR}/tldm.y"
                  "${CMAKE_CURRENT_BINARY_DIR}/parser.cpp"
             COMPILE_FLAGS "-t -y -d")

flex_target(Lexx "${CMAKE_CURRENT_SOURCE_DIR}/tldm.l"
                 "${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp"
            COMPILE_FLAGS "-l")

add_flex_bison_dependency(Lexx Bisx)

add_executable(tldm_parser
    "${BISON_Bisx_OUTPUTS}"
    "${FLEX_Lexx_OUTPUTS}"
    "${TOOLS_TOP}/parsers/src/xml_parser.cpp"
    "${TOOLS_TOP}/parsers/src/cmdline_parser.cpp"
    "${TOOLS_TOP}/parsers/src/file_parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tldm_parser.cpp")

set_target_properties(tldm_parser PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS -DDEBUG")
