cmake_minimum_required(VERSION 3.1.0)
project(SideScrollerSandbox C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB SOURCE_FILES src/*.c)
file(GLOB HEADER_FILES include/*.h)

find_package(LibXml2 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(ALUT REQUIRED)
find_package(Freetype REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(include ../jamengine/include)
add_executable(${PROJECT_NAME} main.c ${SOURCE_FILES} ${HEADER_FILES})
if (UNIX)
	target_link_libraries(${PROJECT_NAME} m $ENV{HOME}/jamengine/lib/libJamEngine.a ${SDL2_LIBRARY} ${ALUT_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${LIBXML2_LIBRARIES} ${OPENAL_LIBRARY} ${FREETYPE_LIBRARIES})
else()
	target_link_libraries(${PROJECT_NAME} C:/Users/lugi1/CLionProjects/jamengine/cmake-build-debug/lib/libJamEngine.a ${SDL2_LIBRARY} ${ALUT_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${LIBXML2_LIBRARIES} ${OPENAL_LIBRARY} ${FREETYPE_LIBRARIES})
endif()