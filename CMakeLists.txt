cmake_minimum_required(VERSION 3.7)
project(dwt_grid)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出到bin目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src)


# target_link_libraries(main protobuf)