include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(DsSkeletonExtract
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ds_skeleton_extract.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dse_utils/dse_utils.cpp"
)

set_target_properties(DsSkeletonExtract PROPERTIES
  LINKER_LANGUAGE CXX)
