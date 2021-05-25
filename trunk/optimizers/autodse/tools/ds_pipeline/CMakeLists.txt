include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(DsPipeline
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ds_pipeline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils/dse_utils.cpp"
)

set_target_properties(DsPipeline PROPERTIES
  LINKER_LANGUAGE CXX)
