# Project architecutre and coding guidelines

## General

- These libraries are meant to be used in embedded C++ projects where recent standard versions are available (at least C++23) and the standard library is available.
- Modern C++ should be used as much as possible to take advantage of the safety and security aspects that it offers.
  - Almost always auto
  - All code should be const correct
  - Constructors should be marked explicit where appropriate
  - The [[nodiscard]] attribute should be applied to functions that return a value
  - Avoid C-style for-loops
    - Use range-based algorithms when possible
    - Use ranged for-loops when iterating through a container
    - Use std::views::iota when iterating over an integer range
  - Use the [[maybe_unused]] attribute to mark unused variables, rather than "(void) x"
- raw pointers and C-style arrays are forbidden
  - arrays should be defined with std::array and passed through a std::ranges parameter using std::span or std::string_view to define subranges as necessary
  - std::optional should be used to convey nullability
- In general, run-time allocations should be limited to initialization, and the library user can be assumed to instantiate any allocating objects or constructs at application startup.
- Preference should be given to performing work at compile time, via consteval, constexpr, etc.
  - Any function or variable that can be declared constexpr should be
- Exceptions should be avoided, as a rule. Any calls to standard library or third party functions that might throw should be wrapped in a try/catch block with specific catch scopes for each possible exception type (i.e., no catch(...)).
  - Functions should be marked noexcept
  - std::expected should be used to convey error conditions and informations to the caller
- Static polymorphism should be preferred over runtime polymorphism, as a rule ("has-a" over "is-a"). The only acceptable reason to use inheritance/subclassing is when an array of a base class type is absolutely necessary to implement critical functionality.
- Concepts should be use to aid template argument deduction
- Assume the host uses little endian byte ordering
- Design should respect the SOLID principles with special emphasis on SRP, OCP, and DIP.
- Design and implementation should respect the Core C++ Guidelines.
- Unit tests written in the Behavior Driven Development style supported by the Catch2 library are critical for demonstrating and maintaining the correctness of the library code.
  - Unit tests must demonstrate that the correct answer is produced under ideal circumstances, and cover error and edge cases.
  - Unit tests should be designed around the required behavior of the code rather than its implementation details, as much as possible.
  - Unit tests should exercise an object via its public interface only.
- Source files should be compact and coherent. 100-200 lines is a good size for most sources, with the "cost function" increasing exponentially as the line count nears and crosses 500 lines. Unit test sources are an exception, but no file should ever exceed 1000 lines.
- Files, classes, and functions should be documented using standard doxygen-style comments:
  - general comments should be placed at the point of declaration (e.g., header file)
    - // @brief
    - // @param[in], @param[out], @param[inout]
    - // @return
    - // @note
  - detailed algorithm descriptions should be included at the point of definition (header or source file), as needed
    - // @note
    - // @todo
  - at some level, "the code is the documentation" and verbose documentation should be treated as a code smell, possibly indicating excessive complexity
- Until C++26 reflection is generally available, Boost::pfr and MagicEnum may be used to provide similar functionality

## Naming conventions

- abbreviations should be avoided, except where there is a strong convention in the "business domain"
- filenames should match the primary object defined and/or implemented in the file
  - flat case
  - namespaces should generall follow the directory structure
- namespaces should be lower_snake_case
- type names should be PascalCase
  - embedded acronyms should be all uppercase
- function names should be lower_snake_case
- variable names should be lower_snake_case
  - private data member names should have a trailing _
- enum members should be PascalCase
- constants in a namespace should be PascalCase
- template parameters should be PascalCase

## Task Workflow

- requirements gathering
- planning and designing
- implementation
- test creation
- analysis for errors, improvements, and adherance to this document
- documentation
- use CMake with Ninja as the build system

## Project structure

- top level project directory should contain conventional directories to organize code
  - **.context** to hold context summaries and other information primarily directed at AI agents
  - **cmake** to store CMake scripts that define libraries or functionality to be included in the main build specification
  - **docs** to hold information primarily meant for human readers
  - **src** to hold application/library sources
    - if the project is an application, its main() function should be defined in src/main.cpp
    - if the project is a library, the src/ level might only contain a CMakeLists.txt
    - each subdirectory under src/ should contain a CMakeLists.txt defining a library that includes the content at that level, recursively
      - content under each subdirectory of src/ should be hierarchical and highly cohesive
  - **tests** to hold all test code, mirroring the first sublevel under to src/
    - a single application called "unittests" contains all unit tests, and the Catch2 library provides functionality to run them all, individually, or in groups
