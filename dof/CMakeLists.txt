# CMakeList.txt : CMake project for dof, include source and define
# project specific logic here.
#

find_package(OpenCV 4.9 REQUIRED)
find_package(OpenGL REQUIRED)

# Simple non robust way to find the librealsense library
if(WIN32)
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(RS_LIBRARY_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x64")  
        set(RS_DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x64")      
        set(GL_LIBRARY_DIR "C:\\tools\\glew-2.1.0\\lib\\Release\\x64")
        set(GL_DLL_DIR "C:\\tools\\glew-2.1.0\\bin\\Release\\x64")    
        set(GLFW_LIBRARY_DIR "C:\\tools\\glfw-3.3.9.bin.WIN64\\lib-vc2022")
        set(GLUT_DLL_DIR "C:\\tools\\freeglut\\bin\\x64")
        set(GLUT_LIBRARY_DIR "C:\\tools\\freeglut\\lib\\x64")

    else()
        set(RS_LIBRARY_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x86")  
        set(RS_DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x86")      
    
        set(GL_LIBRARY_DIR "C:\\tools\\glew-2.1.0\\lib\\Release\\Win32")
        set(GL_DLL_DIR "C:\\tools\\glew-2.1.0\\bin\\Release\\Win32")

        set(GLUT_DLL_DIR "C:\\tools\\freeglut\\bin")
        set(GLUT_LIBRARY_DIR "C:\\tools\\freeglut\\lib")
    endif()

    set(PROJECT_BINARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/${Config}/dof/")       
    
    set(RS_INCLUDE_DIRS "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\include")  
    set(GLEW_INCLUDE_DIRS "C:\\tools\\glew-2.1.0\\include")
    set(GLFW_INCLUDE_DIRS "C:\\tools\\glfw-3.3.9.bin.WIN64\\include")
    set(GLUT_INCLUDE_DIRS "C:\\tools\\freeglut\\include")
endif()


find_library(REALSENSE2_FOUND realsense2 HINTS ${RS_LIBRARY_DIR} REQUIRED)
if(NOT REALSENSE2_FOUND)
    SET(REALSENSE2_FOUND "realsense2")
    message(WARN "Failed to find_library(realsense2)")
endif()

find_library(GL_FOUND glew32 HINTS ${GL_LIBRARY_DIR} REQUIRED)
find_library(GLFW_FOUND glfw3 HINTS ${GLFW_LIBRARY_DIR} REQUIRED)
find_library(GLUT_FOUND freeglut HINTS ${GLUT_LIBRARY_DIR} REQUIRED)


include_directories(dof ${RS_INCLUDE_DIRS})
include_directories(dof ${GLEW_INCLUDE_DIRS})
include_directories(dof ${GLFW_INCLUDE_DIRS})
include_directories(dof ${GLUT_INCLUDE_DIRS})

# Add source to this project's executable.
add_executable (dof "dof.cpp" "dof.h" "example-utils.hpp" "shader_s.h" "stb_image.h" "example.hpp" "stb_easy_font.h")

target_link_libraries(dof PRIVATE
    ${OpenCV_LIBS}
    ${REALSENSE2_FOUND}
    ${GL_FOUND}
    ${GLFW_FOUND}
    ${GLUT_FOUND}
    opengl32.lib
)
target_compile_features(dof PRIVATE cxx_std_17)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dof PROPERTY CXX_STANDARD 20)
endif()

# TODO: might have to copy the dlls from various BIN directories into the exe folder
# because they wont be on the PATH

if(WIN32)
message(STATUS "Adding Post build script to copy realsense2.dll to project's binary folder")
message(STATUS "Will try to copy from ${RS_DLL_DIR} to ${PROJECT_BINARY_OUTPUT_PATH}")
add_custom_command(TARGET dof POST_BUILD        # Adds a post-build event to rs-getting-started
    COMMAND ${CMAKE_COMMAND} -E copy_if_different              # which executes "cmake - E copy_if_different..."
        "${RS_DLL_DIR}/realsense2.dll"                            # <--this is in-file
        ${PROJECT_BINARY_OUTPUT_PATH})                 # <--this is out-file path
add_custom_command(TARGET dof POST_BUILD         
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${GL_DLL_DIR}/glew32.dll"            
        ${PROJECT_BINARY_OUTPUT_PATH})            
add_custom_command(TARGET dof POST_BUILD        # 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${GLUT_DLL_DIR}/freeglut.dll"            
        ${PROJECT_BINARY_OUTPUT_PATH})            

endif()