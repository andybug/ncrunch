
cmake_minimum_required(VERSION 2.8)

SET (CMAKE_C_COMPILER             "/usr/bin/clang")
SET (CMAKE_C_FLAGS                "-Wall -std=c99 -D_GNU_SOURCE")
SET (CMAKE_C_FLAGS_DEBUG          "-g -DNCRUNCH_DEBUG=1")
SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG -DNCRUNCH_RELEASE=1")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
SET (CMAKE_CXX_FLAGS                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

SET (CMAKE_AR      "/usr/bin/llvm-ar")
SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
SET (CMAKE_NM      "/usr/bin/llvm-nm")
SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")





project(ncaacrunch)

set (ncaacrunch_VERSION_MAJOR 1)
set (ncaacrunch_VERSION_MINOR 0)


SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ncaacrunch_SOURCE_DIR}/bin)
#INSTALL(TARGETS ncaacrunch DESTINATION ${ncaacrunch_SOURCE_DIR}/bin)


add_subdirectory(src)

