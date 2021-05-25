include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(DsTiling
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ds_tiling.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils/dse_utils.cpp"
)

set_target_properties(DsTiling PROPERTIES
  LINKER_LANGUAGE CXX)
