cmake_minimum_required(VERSION 3.18)
project(cpp_game LANGUAGES CXX)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC_DIR ${PROJECT_DIR}/src)
set(TEST_DIR ${PROJECT_DIR}/test)
set(LIBS_DIR ${PROJECT_DIR}/libs)

file(TO_NATIVE_PATH "{SRC_DIR}" ${SRC_DIR})

include(ExternalProject)
# include (CTest)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

macro(setup_pkg arg1 arg2 arg3)
  set(PKG_NAME "${arg1}")
  set(PKG_URL "${arg2}")
  set(PKG_VERSION "${arg3}")

  include(FetchContent)
  FetchContent_Declare(
    ${PKG_NAME}
    GIT_REPOSITORY ${PKG_URL}
    GIT_TAG ${PKG_VERSION}
  )

  if(${CMAKE_VERSION} VERSION_LESS 3.14)
    FetchContent_GetProperties(${PKG_NAME})

    if (NOT ${PKG_NAME}_POPULATED)
      set(FETCHCONTENT_QUIET NO)
      FetchContent_Populate(${PKG_NAME})

      set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples

      add_subdirectory(${${PKG_NAME}_SOURCE_DIR} ${${PKG_NAME}_BINARY_DIR})
    endif()
  else()
    FetchContent_MakeAvailable(${PKG_NAME})
  endif()
endmacro()

# raylib
find_package(raylib REQUIRED)
# lua 
add_subdirectory(external/lua)
# sol2
find_package(sol2 REQUIRED)
# doctest
find_package(doctest REQUIRED)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

set(LIBS lua::lib sol2::sol2 raylib)
set(TESTING_LIB doctest)

# output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# glob source files
AUX_SOURCE_DIRECTORY(${SRC_DIR} MAIN_CPP)
AUX_SOURCE_DIRECTORY(${TEST_DIR} TEST_CPP)
set(MAIN_SRC_CPP ${MAIN_CPP})
set(TEST_SRC_CPP ${MAIN_CPP} ${TEST_CPP})
list(FILTER MAIN_SRC_CPP EXCLUDE REGEX "\/test.cpp$")
list(FILTER TEST_SRC_CPP EXCLUDE REGEX "\/main.cpp$")

# targets/executables
# -- main
add_executable(${PROJECT_NAME} ${MAIN_SRC_CPP})
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBS})
# -- test
add_executable(${PROJECT_NAME}_test  ${TEST_SRC_CPP})
target_include_directories(${PROJECT_NAME}_test PRIVATE ${SRC_DIR} ${PROJECT_DIR}/tests)
target_link_libraries(${PROJECT_NAME}_test PRIVATE ${TESTING_LIB} ${LIBS})

# Raylib: Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
