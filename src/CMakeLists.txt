cmake_minimum_required(VERSION 2.8)
project(ncrunch)

include_directories(include)

add_executable(ncrunch main.c hash.c flatf.c teams.c)
target_link_libraries(ncrunch ssl)

