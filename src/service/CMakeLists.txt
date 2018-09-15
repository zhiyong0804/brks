
#cmake version request
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#for backtrace_symbols
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -Wall -m64 -pipe -std=c++11 -lrt -Wno-reorder")

aux_source_directory(. SOURCE_SERVICE_FILES)
ADD_LIBRARY(service ${SOURCE_SERVICE_FILES})

INCLUDE_DIRECTORIES(../../third/include)
INCLUDE_DIRECTORIES(../common)
INCLUDE_DIRECTORIES(../bus)

TARGET_LINK_LIBRARIES(service pthread)
TARGET_LINK_LIBRARIES(service mysqlclient)
