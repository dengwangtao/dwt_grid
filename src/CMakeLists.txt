


include_directories(.)

if(MSVC)
    set(FTXUI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/ftxui/include")
    set(FTXUI_LIB_DIR "${CMAKE_SOURCE_DIR}/third_party/ftxui/lib/Debug")

    include_directories(${FTXUI_INCLUDE_DIR})
    link_directories(${FTXUI_LIB_DIR})

    set(FTXUI_COMPONENT_LIB ftxui-component.lib ftxui-dom.lib ftxui-screen.lib)
endif()

aux_source_directory(. SRCS)

# 生成可执行文件
add_executable(main ${SRCS})

target_link_libraries(main ${FTXUI_COMPONENT_LIB})