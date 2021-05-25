add_library(FinalCodeGen
    "${CMAKE_CURRENT_SOURCE_DIR}/src/final_code_gen.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_update.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_update.cpp"
)

set_target_properties(FinalCodeGen PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org FinalCodeGen)
install(TARGETS FinalCodeGen ARCHIVE DESTINATION lib)
