
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

aux_source_directory(. SOURCE_INTF_FILES)
ADD_LIBRARY(intf ${SOURCE_INTF_FILES})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -Wall -m64 -pipe -std=c++0x -lrt -Wno-reorder -Wdeprecated-declarations")

INCLUDE_DIRECTORIES(../../third/include)
INCLUDE_DIRECTORIES(../common)

LINK_DIRECTORIES(../../third/lib/boost)

TARGET_LINK_LIBRARIES(intf thrift)
TARGET_LINK_LIBRARIES(intf thriftnb)
TARGET_LINK_LIBRARIES(intf event)

