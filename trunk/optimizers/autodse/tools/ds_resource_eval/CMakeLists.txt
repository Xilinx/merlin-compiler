include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

add_library(DsResourceEval
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ds_resource_eval.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ds_resource_eval_xilinx.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ds_resource_eval_intel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils/dse_utils.cpp"
)

set_target_properties(DsResourceEval PROPERTIES
  LINKER_LANGUAGE CXX)
