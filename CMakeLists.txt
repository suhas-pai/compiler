cmake_minimum_required(VERSION 3.10)
project(compiler VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(compiler ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CPack)

target_include_directories(compiler PUBLIC include)
set_target_properties (compiler PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED TRUE
  CXX_EXTENSIONS TRUE
)

execute_process(COMMAND
    llvm-config --cxxflags OUTPUT_VARIABLE LLVM_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND
    llvm-config --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND
    llvm-config --libs all OUTPUT_VARIABLE LLVM_LINK_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

string(REPLACE "-std=c++17 " "" LLVM_CXX_FLAGS ${LLVM_CXX_FLAGS})
string(REPLACE " " ";" LLVM_CXX_FLAGS ${LLVM_CXX_FLAGS})
string(REPLACE " " ";" LLVM_LD_FLAGS ${LLVM_LD_FLAGS})
string(REPLACE " " ";" LLVM_LINK_FLAGS ${LLVM_LINK_FLAGS})

target_compile_options(compiler PUBLIC -stdlib=libc++)
target_compile_options(compiler PUBLIC ${LLVM_CXX_FLAGS})
target_compile_options(compiler PUBLIC -Wall -Wextra -pedantic -Wsign-conversion -Wredundant-decls -Wstrict-overflow=5 -Wundef -Wnull-dereference -funsigned-char -fno-exceptions -Wno-gnu-case-range -Wno-sign-conversion -Wno-unused-parameter)

target_link_options(compiler PUBLIC ${LLVM_LD_FLAGS} ${LLVM_LINK_FLAGS})
target_link_options(compiler PUBLIC -stdlib=libc++)
target_link_options(compiler PUBLIC -fuse-ld=lld)

target_link_libraries(compiler readline)
set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -header-filter=include;
    -checks=*;)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(compiler PRIVATE -fsanitize=address -fsanitize=undefined)
    target_link_options(compiler PRIVATE -fsanitize=address -fsanitize=undefined)

    install_files(. FILES compile_commands.json)
else()
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(compiler PRIVATE -flto -O3)
    endif()
endif()

install(TARGETS compiler DESTINATION ${CMAKE_INSTALL_PREFIX})
