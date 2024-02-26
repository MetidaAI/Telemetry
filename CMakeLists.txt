cmake_minimum_required(VERSION 3.23)
project(Telemetry) #PROJECT METIDA

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} main.cpp cpuState.cpp cpuState.h memoryState.cpp memoryState.h thermalState.cpp thermalState.h processState.cpp processState.h configPaths.h)

target_link_libraries(${PROJECT_NAME} libmetidaCore.so)
target_link_libraries(${PROJECT_NAME} libredis++.so)
target_link_libraries(${PROJECT_NAME} libhiredis.so)
target_link_libraries(${PROJECT_NAME} libtinyConfig.so libtinyLogger.so libtinyUtils.so)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})