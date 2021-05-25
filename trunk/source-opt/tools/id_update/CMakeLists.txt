add_library(IdUpdate
    "${CMAKE_CURRENT_SOURCE_DIR}/src/id_update.cpp"
)

set_target_properties(IdUpdate PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org IdUpdate)
install(TARGETS IdUpdate ARCHIVE DESTINATION lib)
