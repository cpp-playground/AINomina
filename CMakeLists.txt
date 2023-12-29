cmake_minimum_required(VERSION 3.15)
project(placeholder-name CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(
  CMAKE_EXPORT_COMPILE_COMMANDS
  ON
)

# Coverage
set(
  CTEST_COVERAGE_COMMAND
  "llvm-cov"
)
function(target_add_coverage target)
  if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    target_compile_options(
      ${target}
      PRIVATE
        -fprofile-instr-generate
        -fcoverage-mapping
        -mllvm
        -runtime-counter-relocation
    )

    target_link_options(
      ${target}
      PRIVATE
      -fprofile-instr-generate
      -fcoverage-mapping
    )

  endif()
endfunction()

add_compile_options(
  -Wall
  -Wextra
  -pedantic
  -Werror
)

# Setup your project here
# Use target_add_coverage(target) to include target in coverage tests

enable_testing()
# Setup your tests here

#Setup your installation directives here
install(DIRECTORY include/typelist_utils DESTINATION include)
