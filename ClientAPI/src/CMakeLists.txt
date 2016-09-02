cmake_minimum_required(VERSION 3.2)
project(ClientAPI)
set(opencv_DIR )
find_package(opencv)

add_library(ClientDLL SHARED dllmain.cpp MyImg.cpp MyQueue.cpp SenderAPI.cpp stdafx.cpp)
add_definitions(-DSENDERAPI_EXPORTS)
TARGET_LINK_LIBRARIES(ClientDLL ${OpenCV_LIBS})

add_executable(apitest main.cpp)
TARGET_LINK_LIBRARIES(apitest ${OpenCV_LIBS} ClientDLL)

