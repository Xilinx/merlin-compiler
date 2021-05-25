add_library(ArrayDelinearize
    "${CMAKE_CURRENT_SOURCE_DIR}/src/array_delinearize.cpp"
)

set_target_properties(ArrayDelinearize PROPERTIES
  LINKER_LANGUAGE CXX)
add_dependencies(mars_opt_org ArrayDelinearize)
install(TARGETS ArrayDelinearize ARCHIVE DESTINATION lib)
