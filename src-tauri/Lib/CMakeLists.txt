cmake_minimum_required(VERSION 3.10)

# 定义项目名称和类型
project(WinkeyHook)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)

# 添加源文件
file(GLOB SOURCES "src/*.cpp")

# 生成共享库 (动态库)
add_library(winkeyhook SHARED ${SOURCES})

# 指定头文件路径
target_include_directories(winkeyhook PUBLIC include)

# 确保生成 bin 目录
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

SET_TARGET_PROPERTIES(winkeyhook PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin) 
