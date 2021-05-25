add_library(legacy_parsers STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cmdline_parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/file_parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/xml_parser.cpp")

set_target_properties(legacy_parsers PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
set_target_properties(legacy_parsers PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

install(TARGETS legacy_parsers 
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/merlin/legacy/parsers)
