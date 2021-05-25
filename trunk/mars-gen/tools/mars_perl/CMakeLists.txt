include(CheckLibraryExists)

include_directories("${ESLOPT_TOP}/tools/parsers/src/")

# librt is present only on old Linux systems
check_library_exists(rt clock_gettime "" HAVE_LIBRT)
set(system_libs "pthread")
if(HAVE_LIBRT)
    set(system_libs ${system_libs} "rt")
endif()

add_executable(mars_perl
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_perl.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(mars_perl PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS -DIS_DEC=0 -DTOOL_NAME=perl")

target_link_libraries(mars_perl
    ${system_libs})

add_executable(mars_perl_dec
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_perl.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(mars_perl_dec PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS -DIS_DEC=1 -DTOOL_NAME=perl")

target_link_libraries(mars_perl_dec
    ${system_libs})

add_executable(mars_python
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_perl.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(mars_python PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS -DIS_DEC=0 -DTOOL_NAME=python")

target_link_libraries(mars_python
    ${system_libs})

add_executable(mars_python_dec
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_perl.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(mars_python_dec PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS -DIS_DEC=1 -DTOOL_NAME=python")

target_link_libraries(mars_python_dec
    ${system_libs})

add_executable(mars_copy
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_perl.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(mars_copy PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS -DIS_DEC=0 -DTOOL_NAME=cp")

target_link_libraries(mars_copy
    ${system_libs})

add_executable(mars_copy_dec
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_perl.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(mars_copy_dec PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS -DIS_DEC=1 -DTOOL_NAME=cp")

target_link_libraries(mars_copy_dec
    ${system_libs})

add_executable(script_enc
    "${CMAKE_CURRENT_SOURCE_DIR}/enc.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(script_enc PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS -DIS_ENC=1")

add_executable(script_dec
    "${CMAKE_CURRENT_SOURCE_DIR}/enc.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(script_dec PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS -DIS_ENC=0")
