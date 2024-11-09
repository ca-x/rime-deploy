cmake_minimum_required(VERSION 3.10)
project(rime-deploy C)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加源文件
file(GLOB SOURCES "src/*.c")

# 创建可执行文件
add_executable(rime-deploy ${SOURCES})

# 添加头文件目录
target_include_directories(rime-deploy PRIVATE src)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 根据平台添加特定的链接库
if(WIN32)
    target_link_libraries(rime-deploy PRIVATE ws2_32)
elseif(UNIX)
    target_link_libraries(rime-deploy PRIVATE m)
endif()
