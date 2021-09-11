cmake_minimum_required(VERSION 3.0)
project(Interface)
set(CMAKE_CXX_STANDARD 11)

include_directories(include)
include_directories(test)
link_directories(out)


set(SRC_VECTOR src/VectorImpl.cpp src/LoggerImpl.cpp)
set(SRC_SET src/SetImpl.h src/SetImplControlBlock.h
    src/LoggerImpl.cpp src/SetImpl.cpp src/SetImplIterator.cpp src/SetImplControlBlock.cpp)
set(SRC_COMPACT src/CompactImpl.h src/CompactImplControlBlock.h src/MultiIndexImpl.h
    src/LoggerImpl.cpp src/CompactImpl.cpp src/CompactImplIterator.cpp
    src/CompactImplControlBlock.cpp src/MultiIndexImpl.cpp src/CompactImplIterator.cpp)

file(GLOB TEST test/*.cpp)

add_library(Vector SHARED ${SRC_VECTOR})
add_library(Set SHARED ${SRC_SET})
target_link_libraries(Set Vector.dll)
add_library(Compact SHARED ${SRC_COMPACT})
target_link_libraries(Compact Vector.dll)


add_executable(${PROJECT_NAME}-test ${TEST})
target_link_libraries(${PROJECT_NAME}-test Vector Set Compact)
