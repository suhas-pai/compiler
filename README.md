# Dependencies:
    ninja, cmake, llvm (install from homebrew)

# Building & Running (Debug)
    cmake --preset debug
    cd build_debug && ninja
    ./compiler_debug

# Building & Running (Release)
    cmake --preset release
    cd build_release && ninja
    ./compiler