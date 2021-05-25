add_executable(msg_report
    "${CMAKE_CURRENT_SOURCE_DIR}/merlin_msg_report.cpp")

set_target_properties(msg_report PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-DLINUX_COMMANDS")
