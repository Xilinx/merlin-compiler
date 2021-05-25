add_library(MsgTest
    "${CMAKE_CURRENT_SOURCE_DIR}/src/msg_test.cpp"
)

set_target_properties(MsgTest PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org MsgTest)
install(TARGETS MsgTest ARCHIVE DESTINATION lib)
