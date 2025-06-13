

include_directories(.)

aux_source_directory(. SRCS)

# 生成可执行文件
add_executable(main ${SRCS})