cmake_minimum_required(VERSION 3.15)

project(SPR-6)
set(CMAKE_CXX_STANDARD 17)
message(STATUS "CMAKE_CXX_STANDARD:${CMAKE_CXX_STANDARD}")

#HOME
set(HOME ${PROJECT_SOURCE_DIR})
message(STATUS "HOME:${HOME}")

#include
set(HANDERD "${HOME}/include")
include_directories(${HANDERD})
message(STATUS "HANDERD:${HANDERD}")

#lib
set(LIB_DIR "${HOME}/lib")
set(LIBRARY_OUTPUT_PATH "${LIB_DIR}")
message(STATUS "LIBRARY_OUT_PATH:${LIB_DIR}")

#timer
file(GLOB_RECURSE TIMER_SRC "${HOME}/src/timer/*.cpp")
message(STATUS "LIB_SRC:${TIMER_SRC}")
add_library("libtimer" STATIC ${TIMER_SRC})

#spr6
file(GLOB_RECURSE SRP6_SRC "${HOME}/src/srp6/*.cpp")
message(STATUS "SPR6_SRC:${SRP6_SRC}")
add_library("libsrp6" STATIC ${SRP6_SRC})

#sql
file(GLOB_RECURSE SQL_SRC "${HOME}/src/sql/*.cpp")
message(STATUS "SQL_SRC:${SQL_SRC}")
add_library("libsql" STATIC ${SQL_SRC})


#link_library
link_libraries(${LIB_DIR}/libmysql.lib)
link_libraries(${LIB_DIR}/libssl.lib)
link_libraries(${LIB_DIR}/libcrypto.lib)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
link_libraries(${LIB_DIR}/WS2_32.Lib)
endif()

link_libraries(${LIB_DIR}/libsql.lib)
link_libraries(${LIB_DIR}/libtimer.lib)
link_libraries(${LIB_DIR}/libsrp6.lib)


#test
#add_executable(test "${SQL_SRC}")

#executable output path
set(EXECUTABLE_OUTPUT_PATH "${HOME}/bin")
message(STATUS "EXECUTABLE_OUTPUT_PATH:${EXECUTABLE_OUTPUT_PATH}")

#spr6-server
file(GLOB_RECURSE SERVER_SRC "${HOME}/src/srp6-server/*.cpp")
message(STATUS "SERVER_SRC:${SERVER_SRC}")
set(APP_SERVER "SRP-6-server")
message(STATUS "APP_SERVER:${APP_SERVER}")
add_executable(${APP_SERVER} ${SERVER_SRC})

#spr6-cilent
file(GLOB_RECURSE CLIENT_SRC "${HOME}/src/srp6-client/*.cpp")
message(STATUS "CILENT_SRC:${CLIENT_SRC}")
set(APP_CLIENT "SRP-6-cilent")
message(STATUS "APP_CLIENT:${APP_CLIENT}")
add_executable(${APP_CLIENT} ${CLIENT_SRC})


#main
#add_executable(app ${HOME}/src/main.cpp)




