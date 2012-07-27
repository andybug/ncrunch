cmake_minimum_required(VERSION 2.8)
project(ncaacrunch)

set (ncaacrunch_VERSION_MAJOR 1)
set (ncaacrunch_VERSION_MINOR 0)


SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ncaacrunch_SOURCE_DIR}/bin)
#INSTALL(TARGETS ncaacrunch DESTINATION ${ncaacrunch_SOURCE_DIR}/bin)


SET (CMAKE_C_FLAGS "-Wall")


add_subdirectory(src)

