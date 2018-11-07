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
include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/../rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/commands)
include_directories(${CMAKE_SOURCE_DIR}/objects)


include_directories(${CMAKE_SOURCE_DIR}/../googletest/googletest/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/../googletest/googletest [EXCLUDE_FROM_ALL])

#file(MAKE_DIRECTORY "libs")
#message(STATUS "*****")

set(SOURCE_EXE
    ../include/types.cpp
    ../include/log.cpp
    objects/server.cpp
    objects/answer.cpp
    objects/record.cpp
    service_messages.cpp
    store.cpp
)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS system regex thread date_time filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#-------------------------------------------------------------------------------

add_executable(${PROJECT_NAME} ${SOURCE_EXE} swarm.cpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

add_executable(tests ${SOURCE_EXE} unit_tests.cpp)
target_link_libraries(tests ${Boost_LIBRARIES} gtest)
