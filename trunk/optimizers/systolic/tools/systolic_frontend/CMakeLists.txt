include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(SystolicFrontend
  "${CMAKE_CURRENT_SOURCE_DIR}/src/systolic_frontend.cpp")

set_target_properties(SystolicFrontend PROPERTIES
  LINKER_LANGUAGE CXX)
