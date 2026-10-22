cmake_minimum_required(VERSION 3.20.0)
project(ulf-playground)

set(PLATFORM "POSIX" CACHE STRING "Platform to target")
set(LF_MAIN "tutorial/InterruptModal" CACHE STRING "Example to build: its path in src/ without .ulf")

if(NOT DEFINED LOG_LEVEL)
  set(LOG_LEVEL "LF_LOG_LEVEL_WARN")
endif()

# Check that micromode is available in the specified reactor-uc path.
if(NOT EXISTS "$ENV{REACTOR_UC_PATH}/external/micromode/CMakeLists.txt")
  message(FATAL_ERROR "REACTOR_UC_PATH (\"$ENV{REACTOR_UC_PATH}\") is not reactor-uc with micromode. "
                      "Set it to the reactor-uc submodule: export REACTOR_UC_PATH=$(pwd)/reactor-uc")
endif()

get_filename_component(LF_NAME ${LF_MAIN} NAME)
add_executable(${LF_NAME})
include($ENV{REACTOR_UC_PATH}/cmake/ulfc.cmake)

lf_setup()
lf_run_lfc(OUTPUT_DIR ${CMAKE_BINARY_DIR} FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/${LF_MAIN}.ulf)

set(GEN_DIR ${CMAKE_BINARY_DIR}/src-gen/${LF_MAIN})
lf_build_generated_code(${LF_NAME} ${GEN_DIR})

# Enable runtime extensions and specify the generated header file for extensions.
target_compile_definitions(
  reactor-uc PUBLIC LF_RUNTIME_EXTENSIONS=1
                    LF_EXTENSION_CONFIG_HEADER="${GEN_DIR}/lf_extensions.h")
add_subdirectory(${GEN_DIR}/micromode ${CMAKE_BINARY_DIR}/micromode)
target_link_libraries(${LF_NAME} PRIVATE micromode::micromode m)

# The C headers of the mocks.
target_include_directories(${LF_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib)
