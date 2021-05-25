add_library(GenTokenId
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gen_token_id_top.cpp"
)

set_target_properties(GenTokenId PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org GenTokenId)
install(TARGETS GenTokenId ARCHIVE DESTINATION lib)
