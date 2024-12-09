cmake_minimum_required(VERSION 3.10)
project(InventoryManager)

option(TESTING "Enable testing mode" OFF)
option(DEV "Enable developing mode" OFF)

set(EXECUTABLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
set(TESTING_EXECUTABLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp")
set(DATABASE_INIT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/database/initializer.sql")
set(INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts")

# It obtains and builds sqlite source files.
include("${SCRIPTS}/fetch-sqlite.cmake")

# It creates the sqlite database file
include("${SCRIPTS}/create-database.cmake")

# It obtains cxxopts options parser library source
include("${SCRIPTS}/fetch-cxxopts.cmake")

# It creates a interface lib for exposing headers to ${EXECUTABLE}
add_library(add_includes INTERFACE)
target_include_directories(add_includes INTERFACE ${INCLUDES})

# Creates invemanager target
file(GLOB DATABASE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/database/*.cpp")
file(GLOB PRODUCTS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/product/*.cpp")
file(GLOB UTILS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp")

add_library(libinvman SHARED ${DATABASE_SOURCE} ${PRODUCTS_SOURCE} ${UTILS_SOURCE})

# Creates the executable target
if(TESTING)
  message(STATUS "Preparing project for testing mode")
  add_executable(main ${TESTING_EXECUTABLE_PATH})
  target_compile_definitions(main PRIVATE TESTING_MODE)
else()
  message(STATUS "Preparing project for release mode")
  add_executable(main ${EXECUTABLE_PATH})
endif()

# Links the library with the executable
target_link_libraries(main PRIVATE libinvman)

# Definitions
if(DEV)
  target_compile_definitions(main PRIVATE DEV_MODE)
endif()
target_compile_definitions(main PRIVATE DATABASE_INIT_FILE="${DATABASE_INIT_FILE}")

# Dependencies
add_dependencies(main create_database_file)

# Linking libraries
target_link_libraries(main PRIVATE add_includes) # INTERFACE
target_link_libraries(main PRIVATE cxxopts)
target_link_libraries(main PRIVATE sqlite)

# Installation target

include("${SCRIPTS}/installation.cmake")