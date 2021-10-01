cmake_minimum_required( VERSION 2.6 )
project("kimchi-epaper-example")

add_subdirectory("${CMAKE_SOURCE_DIR}/deps/c-periphery")
add_executable(${PROJECT_NAME} epaper-hello-world.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/deps/c-periphery/src)
target_link_libraries(${PROJECT_NAME} periphery)

