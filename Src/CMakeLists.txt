set(PROJECT_NAME PTR-tracker-C)
project(${PROJECT_NAME} C CXX)

set(CMAKE_FIND_ROOT_PATH /usr/arm-none-eabi)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if (UNIX)
  set(TOOLCHAIN_PATH /usr/bin)
  set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-gcc)
  set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-g++)
  set(CMAKE_OBJCOPY ${TOOLCHAIN_PATH}/arm-none-eabi-objcopy)
else ()
  set(TOOLCHAIN_PATH C:/)
  set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-gcc.exe)
  set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-g++.exe)
  set(CMAKE_OBJCOPY ${TOOLCHAIN_PATH}/arm-none-eabi-objcopy.exe)
endif ()

add_definitions(-DSTM32G030xx)
add_definitions(-DSTM32G030F6P6)
add_definitions(-DUSE_STDPERIPH_DRIVER)
add_definitions(-DSUPPORT_CPLUSPLUS)
add_definitions(-D__ASSEMBLY__)

set(LINKER_SCRIPT STM32G030F6PX_FLASH.ld)
set(COMMON_FLAGS "-mcpu=cortex-m0plus -mthumb -Wall -ffunction-sections -fdata-sections -Os -nostartfiles")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map -lstdc++ -Os -mcpu=cortex-m0plus -mthumb -Wl,--gc-sections --specs=nano.specs -T ${LINKER_SCRIPT}")

file(GLOB_RECURSE USER_SOURCES "*.c")
file(GLOB_RECURSE USER_SOURCES_CXX "*.cpp")
file(GLOB_RECURSE USER_HEADERS "*.h")

include_directories(
    hal/cmsis
    hal/cmsis_boot
    hal/stm_lib/inc
    ..
)

add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${USER_SOURCES_CXX} ${USER_HEADERS} ${HAL_SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
    COMMAND ${TOOLCHAIN_PATH}/arm-none-eabi-size ${PROJECT_NAME}.elf
    COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}"
)

set(CMAKE_CXX_STANDARD 11)

add_custom_target(program
    DEPENDS ${PROJECT_NAME}.elf
    COMMAND ${TOOLCHAIN_PATH}/st-flash --reset write ${BIN_FILE} 0x08000000
    COMMENT "Flashing ${BIN_FILE}"
)