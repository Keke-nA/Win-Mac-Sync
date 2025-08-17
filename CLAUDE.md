# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a C++ learning and experimentation repository containing standalone programs that demonstrate various C++20 features and concurrency concepts. The repository consists of independent C++ source files, each exploring different aspects of modern C++ programming.

## Build System

### Compilation
- Use `g++` compiler (g++-15 available)
- Standard compilation command: `g++ -std=c++20 -o <output> <source_file>`
- No formal build system - compile individual files directly
- Each .cpp file is a standalone program

### Running Programs
- Execute compiled programs directly: `./<program_name>`
- No test framework or automated testing setup

## Code Architecture

### Key Components

1. **hello.cpp** - Demonstrates lock-free data structures and memory management
   - Implements a template-based `LockFreeStack<T>` using atomic operations
   - Shows memory allocation patterns with malloc/free
   - Uses `std::atomic<std::shared_ptr<Node<T>>>` for thread-safe operations

2. **coroutine.cpp** - C++20 coroutine generator implementation
   - Custom `Generator<T>` class with promise_type
   - Demonstrates co_yield usage and coroutine lifecycle
   - Shows manual coroutine management with handle_type

3. **coroutine1.cpp** - Asynchronous task with custom awaitable
   - Implements `sleep_for` awaitable type with threading
   - Shows co_await with custom suspension/resumption
   - Demonstrates background task execution

### Design Patterns
- Template-based generic programming
- RAII resource management
- Custom coroutine promise types
- Atomic operations for thread safety
- Manual memory management mixed with smart pointers

### C++20 Features Used
- Coroutines (co_yield, co_await)
- Concepts (implicit through template usage)
- Standard library coroutines support
- Modern atomic operations

## Development Guidelines

### Code Style
- Chinese comments are used in some files for educational purposes
- Mixed style: some files use Chinese, others use English
- Template-heavy code with modern C++ patterns
- Manual memory management in some sections alongside smart pointers

### Testing
- Each file contains its own main() function for testing
- No separate test files or unit tests
- Manual testing by compiling and running individual programs

### Dependencies
- Standard C++20 library only
- No external dependencies
- System threading support for coroutine demonstrations