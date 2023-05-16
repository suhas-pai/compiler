cmake_minimum_required(VERSION 3.5.0)
project(compiler)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/main.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(compiler ${SOURCES})

target_include_directories(compiler PUBLIC include)
set(CXX_FLAGS -Wall -Wextra -pedantic -Wsign-conversion -Wredundant-decls -Wstrict-overflow=5 -Wundef -Wnull-dereference -funsigned-char -fno-exceptions -std=gnu++20 -Wno-gnu-case-range -Wno-sign-conversion -Wno-unused-parameter)

execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE LLVM_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --libs all OUTPUT_VARIABLE LLVM_LINK_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

string(REPLACE "-std=c++17 " "" LLVM_CXX_FLAGS ${LLVM_CXX_FLAGS})
string(REPLACE " " ";" LLVM_CXX_FLAGS ${LLVM_CXX_FLAGS})
string(REPLACE " " ";" LLVM_LD_FLAGS ${LLVM_LD_FLAGS})
string(REPLACE " " ";" LLVM_LINK_FLAGS ${LLVM_LINK_FLAGS})

string(APPEND CXX_FLAGS ";${LLVM_CXX_FLAGS}" )

target_compile_options(compiler PUBLIC ${CXX_FLAGS})

target_link_options(compiler PUBLIC ${LLVM_LD_FLAGS} ${LLVM_LINK_FLAGS})
target_link_libraries(compiler readline)

set_property(TARGET compiler PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -header-filter=include;
    -checks=*;)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(compiler PROPERTIES OUTPUT_NAME "compiler_debug")
    target_link_options(compiler PRIVATE "-fsanitize=undefined")
endif()

install_files(. FILES compile_commands.json)
install(TARGETS compiler DESTINATION ${CMAKE_INSTALL_PREFIX})
