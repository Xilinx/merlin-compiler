add_executable(mars_test
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_test.cpp")

set_target_properties(mars_test PROPERTIES
    LINKER_LANGUAGE CXX)

add_executable(mars_aws
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_aws.cpp")

set_target_properties(mars_aws PROPERTIES
    LINKER_LANGUAGE CXX)

add_executable(mars_flow
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_flow_warning.cpp")

set_target_properties(mars_flow PROPERTIES
    LINKER_LANGUAGE CXX)

add_executable(mars_create_project
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_create_project_warning.cpp")

set_target_properties(mars_create_project PROPERTIES
    LINKER_LANGUAGE CXX)

add_executable(mars_cloud_test
    "${CMAKE_CURRENT_SOURCE_DIR}/mars_cloud_test.cpp")

set_target_properties(mars_cloud_test PROPERTIES
    LINKER_LANGUAGE CXX)
