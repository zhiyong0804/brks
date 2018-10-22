CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

aux_source_directory(.  SOURCE_COMMON_FILES)
ADD_LIBRARY(common ${SOURCE_COMMON_FILES})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -Wall -m64 -pipe -std=c++0x -lrt -Wno-reorder -Wdeprecated-declarations")

INCLUDE_DIRECTORIES(../../third/include)

LINK_DIRECTORIES(../../third/lib/log4cpp)
LINK_DIRECTORIES(../../third/lib/iniparser)

TARGET_LINK_LIBRARIES(common log4cpp)
TARGET_LINK_LIBRARIES(common pthread)
TARGET_LINK_LIBRARIES(common iniparser)
TARGET_LINK_LIBRARIES(common dl)

