SET(CMAKE_SYSTEM_NAME Generic)

SET(FT90X_TOOLCHAIN_ROOT "" CACHE PATH "Path to FTDI 90x Toolchain Root")

SET(CMAKE_C_COMPILER ${FT90X_TOOLCHAIN_ROOT}/FT90x\ Toolchain/Toolchain/tools/bin/ft32-elf-gcc.exe)
SET(CMAKE_CXX_COMPILER ${FT90X_TOOLCHAIN_ROOT}/FT90x\ Toolchain/Toolchain/tools/bin/ft32-elf-g++.exe)
SET(CMAKE_LINKER ${FT90X_TOOLCHAIN_ROOT}/FT90x\ Toolchain/Toolchain/tools/bin/ft32-elf-ld.exe)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -L\"${FT90X_TOOLCHAIN_ROOT}/FT90x\ Toolchain/Toolchain/hardware/lib\" -Wl,--gc-sections -Wl,--entry=_start")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common -ffunction-sections -fdata-sections -Wall -Os -ffunction-sections")

include_directories(${FT90X_TOOLCHAIN_ROOT}/FT90x\ Toolchain/Toolchain/hardware/include)


function(add_ft90x_executable EXECUTABLE_NAME)
    message(STATUS ${ARGN})
    add_executable(${EXECUTABLE_NAME}.elf EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${EXECUTABLE_NAME}.elf -lft900 -lc -lstub)
    add_custom_command(
        OUTPUT ${EXECUTABLE_NAME}.bin
        COMMAND
            ${CMAKE_OBJCOPY} --output-target binary ${EXECUTABLE_NAME}.elf  ${EXECUTABLE_NAME}.bin
        DEPENDS ${EXECUTABLE_NAME}.elf
    )
    add_custom_command(
        OUTPUT ${EXECUTABLE_NAME}.hex
        COMMAND
            ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_NAME}.elf ${EXECUTABLE_NAME}.hex
        DEPENDS ${EXECUTABLE_NAME}.elf
    )
    add_custom_target(
        ${EXECUTABLE_NAME}
        ALL
        DEPENDS ${EXECUTABLE_NAME}.hex ${EXECUTABLE_NAME}.bin
    )
endfunction(add_ft90x_executable)