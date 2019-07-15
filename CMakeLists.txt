cmake_minimum_required(VERSION 2.8)

project(swarm)

find_package(Git REQUIRED)

execute_process(
    COMMAND ${GIT_EXECUTABLE} log HEAD~1..HEAD --pretty=oneline
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_HASH_TEMP
)

string(REGEX MATCH [0-9a-f]+ GIT_HASH "${GIT_HASH_TEMP}")

add_definitions(-DDAEMON=\"chatd\" -DGIT_HASH=\"${GIT_HASH}\")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(${CMAKE_SOURCE_DIR})
file(GLOB SOURCE "${CMAKE_SOURCE_DIR}/*.cpp")
include_directories(${CMAKE_SOURCE_DIR}/commands)
file(GLOB SOURCE_COMMANDS "${CMAKE_SOURCE_DIR}/commands/*.cpp")
include_directories(${CMAKE_SOURCE_DIR}/objects)
file(GLOB SOURCE_OBJECTS "${CMAKE_SOURCE_DIR}/objects/*.cpp")

include_directories(${CMAKE_SOURCE_DIR}/../include)
add_subdirectory(${CMAKE_SOURCE_DIR}/../include ${CMAKE_SOURCE_DIR}/../include)

include_directories(${CMAKE_SOURCE_DIR}/../rapidjson/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/../rapidjson ${CMAKE_SOURCE_DIR}/../rapidjson)

include_directories(${CMAKE_SOURCE_DIR}/../googletest/googletest/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/../googletest/googletest ${CMAKE_SOURCE_DIR}/../googletest/googletest)

include_directories(${CMAKE_SOURCE_DIR}/../websocketpp)
add_subdirectory(${CMAKE_SOURCE_DIR}/../websocketpp ${CMAKE_SOURCE_DIR}/../websocketpp)

#file(MAKE_DIRECTORY "libs")
#message(STATUS "*****")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS system regex thread date_time filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#-------------------------------------------------------------------------------

add_executable(${PROJECT_NAME} ${SOURCE} ${SOURCE_COMMANDS} ${SOURCE_OBJECTS} swarm.cpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pslib)

add_executable(tests ${SOURCE} ${SOURCE_COMMANDS} ${SOURCE_OBJECTS} unit_tests.cpp)
target_link_libraries(tests ${Boost_LIBRARIES} gtest pslib)
