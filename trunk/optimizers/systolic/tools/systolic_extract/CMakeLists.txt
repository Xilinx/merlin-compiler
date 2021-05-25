
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${OPTIMIZER_HOME}/systolic/tools/systolic_frontend/src")

add_library(SystolicExtract
    "${CMAKE_CURRENT_SOURCE_DIR}/src/systolic_extract.cpp"
    "${OPTIMIZER_HOME}/systolic/tools/systolic_frontend/src/SystolicData.cpp"
    "${OPTIMIZER_HOME}/systolic/tools/systolic_frontend/src/index_transform_codegen.cpp"
    )

set_target_properties(SystolicExtract PROPERTIES
      LINKER_LANGUAGE CXX)
