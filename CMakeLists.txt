cmake_minimum_required(VERSION 3.14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Configure toolchain
set(CMAKE_TOOLCHAIN_FILE tm4c123gh6pm-toolchain)

project(heli)

include(tm4c123gh6pm)

# Download libraries
include(TivaWare)
include(OrbitOLED)
include(FreeRTOS)
include(TraceRecorder)

# Build source files
file(GLOB SOURCES src/*.c src/*.h)
add_executable(${CMAKE_PROJECT_NAME}.axf ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME}.axf TivaWare OrbitOLED FreeRTOS TraceRecorder)