cmake_minimum_required(VERSION 3.2)
project(link1)
set(CXX_STANDARD_REQUIRED 23)

file(GLOB_RECURSE SOURCES source/*.cpp)

find_package(glfw3 REQUIRED CONFIG)

message("${CMAKE_BINARY_DIR}/webgpu")
add_subdirectory(${CMAKE_BINARY_DIR}/webgpu)

add_executable(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
#set_property(TARGET webgpu PROPERTY IMPORTED_LOCATION "${CMAKE_ROOT}/webgpu/linux-x86_64/libwgpu.so")
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE webgpu)
target_include_directories(${PROJECT_NAME} PUBLIC include)
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_copy_webgpu_binaries(${PROJECT_NAME})

