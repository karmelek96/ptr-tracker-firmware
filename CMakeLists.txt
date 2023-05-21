cmake_minimum_required(VERSION 3.6)
project(PTR-tracker-C)

SET(CMAKE_SYSTEM_NAME "Generic")
SET(CMAKE_SYSTEM_VERSION 1)

SET(UNIX 1)

include_directories(inc)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/CMSIS/Device/ST/STM32G0xx/Include)
link_directories(src)
add_subdirectory(src)