add_library(legacy_ir
    "${TOOLS_TOP}/ir/pltf_annotate.cpp"
    "${TOOLS_TOP}/ir/PolyModel.cpp"
    "${TOOLS_TOP}/ir/tldm_annotate.cpp")

file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set_target_properties(legacy_ir PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

install(TARGETS legacy_ir
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/merlin/legacy/ir)
