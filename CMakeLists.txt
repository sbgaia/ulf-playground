cmake_minimum_required(VERSION 3.20.0)
project(ulf-playground)

# Build one example: cmake -B build/InterruptModal -DLF_MAIN=InterruptModal
set(PLATFORM "POSIX" CACHE STRING "Platform to target")
set(LF_MAIN "InterruptModal" CACHE STRING "Example to build, a file of src/ without .ulf")

if(NOT DEFINED LOG_LEVEL)
  set(LOG_LEVEL "LF_LOG_LEVEL_WARN")
endif()

# The reactor-uc submodule, unless REACTOR_UC_PATH points to another checkout.
if(NOT DEFINED ENV{REACTOR_UC_PATH})
  set(ENV{REACTOR_UC_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/reactor-uc)
endif()

add_executable(${LF_MAIN})
include($ENV{REACTOR_UC_PATH}/cmake/ulfc.cmake)

lf_setup()
lf_run_lfc(OUTPUT_DIR ${CMAKE_BINARY_DIR} FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/${LF_MAIN}.ulf)

set(GEN_DIR ${CMAKE_BINARY_DIR}/src-gen/${LF_MAIN})
lf_build_generated_code(${LF_MAIN} ${GEN_DIR})

# Enable runtime extensions and specify the generated header file for extensions.
target_compile_definitions(
  reactor-uc PUBLIC LF_RUNTIME_EXTENSIONS=1
                    LF_EXTENSION_CONFIG_HEADER="${GEN_DIR}/lf_extensions.h")
add_subdirectory(${GEN_DIR}/micromode ${CMAKE_BINARY_DIR}/micromode)
target_link_libraries(${LF_MAIN} PRIVATE micromode::micromode)
