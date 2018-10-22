#NodeServer cmake config file for src

#cmake version request
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#project name
PROJECT(brks)

#Head file path
INCLUDE_DIRECTORIES(../third/include)
INCLUDE_DIRECTORIES(./service)
INCLUDE_DIRECTORIES(./intf)
INCLUDE_DIRECTORIES(./common)
INCLUDE_DIRECTORIES(./bus)

#library path
LINK_DIRECTORIES(../third/lib/jsoncpp)
LINK_DIRECTORIES(../third/lib/tinyxml)
LINK_DIRECTORIES(../third/lib/log4cpp)
LINK_DIRECTORIES(../third/lib/thrift)
LINK_DIRECTORIES(../third/lib/iniparser)
LINK_DIRECTORIES(./common)
LINK_DIRECTORIES(./bus)
LINK_DIRECTORIES(./intf)
LINK_DIRECTORIES(./service)

#debug mode
SET(CMAKE_BUILD_TYPE Debug)

#################################################################
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR} SOURCE_FILES)
ADD_EXECUTABLE(brks ${SOURCE_FILES})


#for backtrace_symbols
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -Wall -g3 -m64 -pipe -std=c++0x -lrt -Wno-reorder -Wdeprecated-declarations -fpermissive")

TARGET_LINK_LIBRARIES(brks log4cpp)
TARGET_LINK_LIBRARIES(brks thrift)
TARGET_LINK_LIBRARIES(brks thriftnb)
TARGET_LINK_LIBRARIES(brks event)
TARGET_LINK_LIBRARIES(brks jsoncpp)
TARGET_LINK_LIBRARIES(brks libtinyxml.a)
TARGET_LINK_LIBRARIES(brks pthread)
TARGET_LINK_LIBRARIES(brks mysqlclient)
TARGET_LINK_LIBRARIES(brks curl)
TARGET_LINK_LIBRARIES(brks iniparser)
TARGET_LINK_LIBRARIES(brks dl)

TARGET_LINK_LIBRARIES(brks bus)
TARGET_LINK_LIBRARIES(brks intf)
TARGET_LINK_LIBRARIES(brks common)
TARGET_LINK_LIBRARIES(brks service)


#子目�?
ADD_SUBDIRECTORY(bus)
ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(intf)
ADD_SUBDIRECTORY(service)

#install
#SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
INSTALL(TARGETS brks DESTINATION bin)
