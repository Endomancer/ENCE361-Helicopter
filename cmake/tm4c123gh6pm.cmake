# Processor specific definitions
add_compile_definitions(PART_TM4C123GH6PM TARGET_IS_TM4C123_RA1 gcc)

# Define binary command and target
add_custom_command(
    OUTPUT ${CMAKE_PROJECT_NAME}.bin
    DEPENDS ${CMAKE_PROJECT_NAME}.axf
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.axf ${CMAKE_PROJECT_NAME}.bin
)
add_custom_target(binary DEPENDS ${CMAKE_PROJECT_NAME}.bin)

# Select OS specific programming tool
if(CMAKE_HOST_UNIX)
    set(FLASH_EXECUTABLE lm4flash)
elseif(CMAKE_HOST_WIN32)
    set(FLASH_EXECUTABLE lmflash -v -r -q ek-tm4c123gxl)
endif()

# Define program target
if(FLASH_EXECUTABLE)
    add_custom_target(program DEPENDS ${CMAKE_PROJECT_NAME}.bin
        COMMAND ${FLASH_EXECUTABLE} ${CMAKE_PROJECT_NAME}.bin
    )
endif()