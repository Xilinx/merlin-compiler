add_library(DependencyResolve
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dependency_resolve.cpp"
)

set_target_properties(DependencyResolve PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org DependencyResolve)
install(TARGETS DependencyResolve ARCHIVE DESTINATION lib)
