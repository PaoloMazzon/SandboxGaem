cmake_minimum_required(VERSION 3.1.0)
project(SideScrollerSandbox C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

find_package(LibXml2 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(ALUT REQUIRED)
include_directories(include /usr/include/SDL2 ${LibXml2_INCLUDE_DIR} ${OPENAL_INCLUDE_DIR} ~/jamengine/include)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(${PROJECT_NAME} main.c src/Player.c)
target_link_libraries(${PROJECT_NAME} m /home/pdmazzon/jamengine/libJamEngine.a ${SDL2_LIBRARY} ${ALUT_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${LIBXML2_LIBRARIES} ${OPENAL_LIBRARY})
