SET(execName PCL_zed)
cmake_minimum_required( VERSION 2.8 )
project(${execName})

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 OLD)
	cmake_policy(SET CMP0015 OLD)
endif(COMMAND cmake_policy)

SET(EXECUTABLE_OUTPUT_PATH ".")    
SET(VERSION_REQ_CUDA "8.0")

IF(WIN32) # Windows
    if (CMAKE_CL_64)  
        SET(ZED_INCLUDE_DIRS $ENV{ZED_INCLUDE_DIRS})
        SET(ZED_LIBRARIES $ENV{ZED_LIBRARIES_64})
        SET(ZED_LIBRARY_DIR $ENV{ZED_LIBRARY_DIR})
    else()
        message(FATAL_ERROR "You've selected the 32bit version of ${CMAKE_GENERATOR}. \n Please delete the cache (file->Delete Cache) and use the 64bit version. (${CMAKE_GENERATOR} Win64)")
    endif()
ELSE() # Linux
    find_package(ZED 2.1 REQUIRED)
    SET(SPECIAL_OS_LIBS "pthread" "X11")
    add_definitions(-std=c++0x -O3 -pthread)
ENDIF(WIN32)
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )
set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib )

# 添加cmake模块以使用g2o
 list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules )

# OpenCV
find_package( OpenCV 3.1 REQUIRED )
find_package(CUDA ${VERSION_REQ_CUDA} REQUIRED)
find_package(PCL)
find_package( G2O REQUIRED )
find_package( CSparse REQUIRED )

include_directories(${OpenCV_INCLUDE_DIRS} )
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${PCL_INCLUDE_DIRS})
include_directories(${ZED_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${G2O_INCLUDE_DIRS}
                    ${CSPARSE_INCLUDE_DIR}
                    "/usr/include/eigen3/")

add_subdirectory( src )
add_subdirectory( test )


