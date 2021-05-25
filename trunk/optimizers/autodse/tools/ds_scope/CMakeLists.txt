include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(DsScope
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ds_scope.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils/dse_utils.cpp"
)

set_target_properties(DsScope PROPERTIES
  LINKER_LANGUAGE CXX)
