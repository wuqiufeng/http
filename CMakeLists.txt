#cmake_minimum_required(VERSION 3.7)
project(HTTP)
#使用变量
set(target HTTP.bin)

#设置头文件目录
include_directories(include)
#设置库目录
link_directories(./lib)

#搜索目录
aux_source_directory(./src src)

add_executable(${target} ${src})
#链接动态库
target_link_libraries(${target} pthread curl http_parser)
