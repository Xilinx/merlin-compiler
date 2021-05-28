add_library(PerformanceEstimation
    "${CMAKE_CURRENT_SOURCE_DIR}/src/performance_estimation.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/performance_estimation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/fast_dse.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/latency_estimation.cpp"
)

set_target_properties(PerformanceEstimation PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org PerformanceEstimation)
install(TARGETS PerformanceEstimation ARCHIVE DESTINATION lib)
