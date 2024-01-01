# 设置项目名称
project(raytracing)

# 指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置C++标准版本为C++11
set(CMAKE_CXX_STANDARD 11)

# 添加可执行文件并指定源文件
add_executable(main main.cpp)

# 添加Assimp库的链接
find_package(Assimp REQUIRED)
target_include_directories(main PRIVATE ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${ASSIMP_LIBRARIES})

# 添加OpenCV库的链接
find_package(OpenCV REQUIRED)
target_include_directories(main PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${OpenCV_LIBS})

# 添加多线程支持
find_package(Threads REQUIRED)
target_compile_options(main PRIVATE "-pthread")
target_link_libraries(main PRIVATE Threads::Threads)
