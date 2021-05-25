include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(DsBitwidthOpt
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ds_bitwidth_opt.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils/dse_utils.cpp"
)

set_target_properties(DsBitwidthOpt PROPERTIES
  LINKER_LANGUAGE CXX)
