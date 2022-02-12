file(GLOB source
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

file(GLOB public_h
     LIST_DIRECTORIES FALSE
     RELATIVE "${build_include_core}"
     "${build_include_core}/*.h")

set(public_h_build)
set(public_h_install)

foreach(h IN LISTS public_h)
    file(RELATIVE_PATH _path ${CMAKE_CURRENT_LIST_DIR} "${build_include_core}/${h}")
    list(APPEND public_h_build   "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${_path}>")  # 相对路径的转换, 此处只能使用相对路径
    list(APPEND public_h_install "$<INSTALL_INTERFACE:${install_include_core}/${h}>")
endforeach()

add_library(core-shared SHARED "")
add_library(core-static STATIC "")

foreach(tgt core-shared core-static)
    target_sources(${tgt} PRIVATE ${source} PUBLIC ${public_h_build} ${public_h_install})
    target_include_directories(${tgt} PUBLIC ${build_include} ${install_include})
    set_target_properties(${tgt} PROPERTIES PUBLIC_HEADER "${public_h_build}")
    define_filename(${tgt})
endforeach()

set_target_properties(core-shared PROPERTIES OUTPUT_NAME "aFuncore")
set_target_properties(core-static PROPERTIES OUTPUT_NAME "aFuncore-s")

target_link_libraries(core-shared PUBLIC tool-shared code-shared)
target_link_libraries(core-static PUBLIC tool-static code-static)
#
#install(TARGETS core-shared core-static
#        EXPORT aFunlang
#        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT base-runtime
#        ARCHIVE DESTINATION ${INSTALL_LIBDIR} COMPONENT dev
#        LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT base-runtime
#        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev)