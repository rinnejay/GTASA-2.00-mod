cmake_minimum_required(VERSION 3.10.2)
project("GtaHook")

add_library(hooker SHARED hooker.cpp)
add_library(target SHARED target.cpp)

find_library(log-lib log)

target_link_libraries(hooker ${log-lib} dl)
target_link_libraries(target ${log-lib})