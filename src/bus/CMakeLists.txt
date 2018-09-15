
#cmake version request
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#for backtrace_symbols
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -Wall -m64 -pipe -std=c++0x -lrt -Wno-reorder")

aux_source_directory(. SOURCE_BUS_FILES)
ADD_LIBRARY(bus ${SOURCE_BUS_FILES})

INCLUDE_DIRECTORIES(../../third/include)
INCLUDE_DIRECTORIES(../common)
INCLUDE_DIRECTORIES(../service)

LINK_DIRECTORIES(../common)

TARGET_LINK_LIBRARIES(bus pthread)
TARGET_LINK_LIBRARIES(bus jsoncpp)
TARGET_LINK_LIBRARIES(bus curl)
TARGET_LINK_LIBRARIES(bus common)
