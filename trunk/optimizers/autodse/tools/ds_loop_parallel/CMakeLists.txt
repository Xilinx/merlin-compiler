include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(DsLoopParallel
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ds_loop_parallel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils/dse_utils.cpp"
)

set_target_properties(DsLoopParallel PROPERTIES
  LINKER_LANGUAGE CXX)
