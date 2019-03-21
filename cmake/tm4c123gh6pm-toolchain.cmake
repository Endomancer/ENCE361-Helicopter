#Set cross compilation information
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# No CMake support for C18
set(C_VERSION "-std=gnu18")

set(OPT "-Os")
set(CPU "-mcpu=cortex-m4")
set(FPU "-mfpu=fpv4-sp-d16 -mfloat-abi=hard")

set(CMAKE_ASM_FLAGS "-mthumb ${CPU} ${FPU} -MD")
set(CMAKE_C_FLAGS   "-mthumb ${CPU} ${FPU} ${OPT} -ffunction-sections -fdata-sections -MD -Wall -pedantic ${C_VERSION}")
set(CMAKE_CXX_FLAGS "-mthumb ${CPU} ${FPU} ${OPT} -ffunction-sections -fdata-sections -MD -Wall -pedantic -fno-exceptions -fno-rtti")
set(CMAKE_EXE_LINKER_FLAGS "-T${PROJECT_SOURCE_DIR}/cmake/tm4c123gh6pm.lds -specs=${PROJECT_SOURCE_DIR}/cmake/tiva.specs")

# GCC toolchain prefix
set(TOOLCHAIN_PREFIX "arm-none-eabi")

set(CMAKE_C_COMPILER    ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER  ${TOOLCHAIN_PREFIX}-as)
set(CMAKE_AR            ${TOOLCHAIN_PREFIX}-ar)
set(CMAKE_OBJCOPY       ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP       ${TOOLCHAIN_PREFIX}-objdump)