cmake_minimum_required(VERSION 3.7)
project(dwt_grid)





if(MSVC)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    add_compile_options(/source-charset:utf-8 /execution-charset:utf-8)
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 设置输出到bin目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


add_subdirectory(src)


# target_link_libraries(main protobuf)