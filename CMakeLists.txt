cmake_minimum_required(VERSION 3.10)
project(InventoryManager)

option(TESTING "Enable testing mode" OFF) # Loads a diferent entry point for the program
option(DEV "Enable developing mode" OFF)  # Enables some features like an initializing database method

## Compilation CMake variables ##

# Determines the absolute path the begining of the program
set(EXECUTABLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
set(TESTING_EXECUTABLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp")

# Determines the absolute path to the sql database initializer code
set(DATABASE_INIT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/scripts/database/initializer.sql")

# Determines the absolute path to header files to compile the program
set(HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include")

## Installation / Uninstallation CMake variables ##

# Determines the absolute path to CMake installation / uninstallation scripts
set(SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake")

# Determines the installation path for library libinvmanl.so header files
set(INST_HEADERS_DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libinvmanl")

# Determines the installation path for the program executable
set(INST_RUNTIME_DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

# Determines the installation path for libinvmanl.so
set(INST_LIBRARY_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

## Includes the library compilation scripts ( sqlite3 and cxxopts ) ##

# It obtains and builds sqlite source files.
include("${SCRIPTS}/fetch-sqlite.cmake")

# It creates the sqlite database file
include("${SCRIPTS}/create-database.cmake")

# It obtains cxxopts options parser library source
include("${SCRIPTS}/fetch-cxxopts.cmake")

# It creates a interface lib for exposing headers to ${EXECUTABLE}
add_library(add_includes INTERFACE)
target_include_directories(add_includes INTERFACE ${HEADERS})

# Creates invmanl target
file(GLOB DATABASE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/database/*.cpp")
file(GLOB PRODUCTS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/product/*.cpp")

add_library(invmanl SHARED ${DATABASE_SOURCE} ${PRODUCTS_SOURCE} ${UTILS_SOURCE})
target_link_libraries(invmanl PUBLIC add_includes)
target_link_libraries(invmanl PUBLIC cxxopts)
target_link_libraries(invmanl PUBLIC sqlite)

# Creates the executable target
if(TESTING)
  message(STATUS "Preparing project for testing mode")
  add_executable(invman ${TESTING_EXECUTABLE_PATH})
  target_compile_definitions(invman PRIVATE TESTING_MODE)
else()
  message(STATUS "Preparing project for release mode")
  add_executable(invman ${EXECUTABLE_PATH})
endif()

# Links the library with the executable
target_link_libraries(invman PRIVATE invmanl)

# Definitions
if(DEV)
  target_compile_definitions(invman PRIVATE DEV_MODE)
endif()

target_compile_definitions(invman PRIVATE DATABASE_INIT_FILE="${DATABASE_INIT_FILE}")

# Dependencies
add_dependencies(invman create_database_file)

# Creates installation / desinstallation targets
include("${SCRIPTS}/create-install-target.cmake")
include("${SCRIPTS}/create-uninstall-target.cmake")