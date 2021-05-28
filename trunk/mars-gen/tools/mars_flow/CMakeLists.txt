include_directories("${ESLOPT_TOP}/tools/parsers/src")
link_directories( 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${ESLOPT_TOP}/../flexlm")

add_executable(merlin_flow
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_flow_x64.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(merlin_flow PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

target_link_libraries(merlin_flow
    dl
    pthread)

add_executable(merlin_safe_exec
    "${CMAKE_CURRENT_SOURCE_DIR}/merlin_safe_exec.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(merlin_safe_exec PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

target_link_libraries(merlin_safe_exec
    pthread)

add_executable(marscc
    "${CMAKE_CURRENT_SOURCE_DIR}/marscc.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(marscc PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(display
    "${CMAKE_CURRENT_SOURCE_DIR}/display.cpp")

set_target_properties(display PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(mars_exec_cloud
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_exec_cloud.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(mars_exec_cloud PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(merlin_flow_cloud
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_flow_cloud.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(merlin_flow_cloud PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(mars_flow_x64
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_flow_x64.cpp"
    "${ESLOPT_TOP}/tools/parsers/src/file_parser.cpp")

set_target_properties(mars_flow_x64 PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

target_link_libraries(mars_flow_x64
    dl
    pthread)

add_executable(merlin_create_project
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_create_project_x64.cpp")

set_target_properties(merlin_create_project PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")
add_executable(mars_env_loader
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_env_load.cpp")

set_target_properties(mars_env_loader PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(mars_opt
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_opt_wrapper.cpp")

set_target_properties(mars_opt PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(merlincc
    "${CMAKE_CURRENT_SOURCE_DIR}/merlincc.cpp")

set_target_properties(merlincc PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(mars_install
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_install.cpp")

set_target_properties(mars_install PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(merlin_frontend
    "${CMAKE_CURRENT_SOURCE_DIR}/merlin_frontend.cpp")

set_target_properties(merlin_frontend PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(merlin_midend
    "${CMAKE_CURRENT_SOURCE_DIR}/merlin_midend.cpp")

set_target_properties(merlin_midend PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(merlin_backend
    "${CMAKE_CURRENT_SOURCE_DIR}/merlin_backend.cpp")

set_target_properties(merlin_backend PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

add_executable(merlinprof
    "${CMAKE_CURRENT_SOURCE_DIR}/merlinprof.cpp")

set_target_properties(merlinprof PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")

link_directories( 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ) 

add_executable(merlin_driver
    "${CMAKE_CURRENT_SOURCE_DIR}/merlin_driver.cpp")

target_link_libraries(merlin_driver
    dl
    pthread)

set_target_properties(mars_opt PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")
