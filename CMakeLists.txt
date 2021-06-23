cmake_minimum_required(VERSION 3.15)
project(Chip_8)
set(CMAKE_CXX_STANDARD 17)

include_directories(OpenGL)
include_directories(OpenGL/include) # OpenGL/include has to contain the required OpenGL's .h files
include_directories(OpenGL/lib) # OpenGL/lib has to contain the required OpenGL's .lib files
# glut32.dll must be present in "project-directory/OpenGL/dll/"


add_custom_target(glutdlllib
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/OpenGL/dll/glut32.dll ${CMAKE_BINARY_DIR}
        )
# required .lib files to be copied into compiler's proper directory
set(OpenGlLibs glaux glu32 glui32 glut32 opengl32)
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/chip-8.cpp )
