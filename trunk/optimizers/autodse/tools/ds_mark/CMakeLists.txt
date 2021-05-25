include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(DsMark
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ds_mark.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils/dse_utils.cpp"
)

set_target_properties(DsMark PROPERTIES
  LINKER_LANGUAGE CXX)
