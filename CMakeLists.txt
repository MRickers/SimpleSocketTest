cmake_minimum_required(VERSION 3.27)
project(simpleSocketTest)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(SimpleSocket)
add_subdirectory(raylib)

add_executable(simpleSocketServerTest simpleSocketTestServer.cpp
        src/ServerApp.cpp
        src/ServerApp.hpp
        src/utils.hpp)
target_include_directories(simpleSocketServerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(simpleSocketServerTest simpleSocket raylib)

add_executable(simpleSocketClientTest simpleSocketTestClient.cpp
        src/ClientApp.cpp
        src/ClientApp.hpp
        src/utils.hpp

)
target_include_directories(simpleSocketClientTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(simpleSocketClientTest simpleSocket raylib)