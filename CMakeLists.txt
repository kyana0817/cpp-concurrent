cmake_minimum_required(VERSION 3.28.3)
project(LearnAsync LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-g)

add_library(shared shared/tcp.cpp)

add_executable(epoll epoll/epoll.cpp)
add_executable(unused_epoll epoll/unused.cpp)

target_link_libraries(epoll PRIVATE shared)
target_link_libraries(unused_epoll PRIVATE shared)
