cmake_minimum_required(VERSION 3.15)
project(Chip_8)
set(CMAKE_CXX_STANDARD 20)
find_package(SDL2 REQUIRED)
include_directories( ${SDL2_INCLUDE_DIRS} )
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/chip-8.cpp )
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
