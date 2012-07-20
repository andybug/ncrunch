cmake_minimum_required(VERSION 2.8)
project(ncaacrunch)

include_directories(include)

add_executable(ncaacrunch main.c)
target_link_libraries(ncaacrunch ssl)

