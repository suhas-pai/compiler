# Compiler

A compiler for a so-far unnamed systems & functional programming language.
Takes inspiration from TypeScript, Zig, and Rust.

## Plan and Wishlist

In the future, the following features will hopefully be implemented:

 1. Interfaces
 2. Types as values Meta-programming
 3. Algebraic Effects
 4. Refinement and Higher Kinded Types (HKT)

## Building

### Dependencies

Only supports building on macOS so far. Install the following packages from Homebrew:

    ninja, cmake, llvm, lld

### Building & Running (Debug)

    cmake --preset debug
    cd build_debug && ninja
    ./compiler

### Building & Running (Release)

    cmake --preset release
    cd build_release && ninja
    ./compiler
