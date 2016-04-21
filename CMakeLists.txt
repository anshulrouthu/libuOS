
cmake_minimum_required(VERSION 2.8.4)
project(uOS)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release." FORCE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_ROOT}/cmake_config/find_packages)

# Helper macros
macro(addtolist list file condition)
    if(${condition})
        list(APPEND ${list} ${file})
    endif()
endmacro()

macro(addtodefinition def)
    if(${def})
        add_definitions(-D${def})
    endif()
endmacro()

# Set option required for the project
option(ENABLE_MEMMGR "Enable memory manager" ON)
option(ENABLE_TSK_SCHEDULER "Enable task scheduler" ON)

addtodefinition(ENABLE_MEMMGR)
addtodefinition(ENABLE_TSK_SCHEDULER)
add_definitions(-DBUILD_MODE=${CMAKE_BUILD_TYPE})

# Include dirs
list(APPEND SRC_INC_DIR memmgr)
list(APPEND SRC_INC_DIR tsk_scheduler)

# Source files
addtolist(SRC_FILES memmgr/memmgr.c                     "ENABLE_MEMMGR")
addtolist(SRC_FILES tsk_scheduler/tsk_scheduler.c       "ENABLE_TSK_SCHEDULER")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${SRC_INC_DIR})

# create lib
add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
