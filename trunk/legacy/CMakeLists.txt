include_directories(
  "${TOOLS_TOP}/parsers/src"
  "${TOOLS_TOP}/wrapper_gen/src"
  "${TOOLS_TOP}/ir"
  "${TOOLS_TOP}/loop_parallel"
  "${TOOLS_TOP}/memory_burst"
  "${TOOLS_TOP}/coarse_pipeline"
  "${TOOLS_TOP}/auto_pipeline"
  "${TOOLS_TOP}/reduction"
  "${TOOLS_TOP}/mhs_gen")

add_subdirectory("tools/parsers")
add_subdirectory("tools/ir")
add_subdirectory("tools/ir/test")
add_subdirectory("tools/utils")
