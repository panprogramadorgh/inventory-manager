cmake_minimum_required(VERSION 3.10)
project(InventoryManager)

# Enables the C++20 standard and disables specific features of the compiler
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MAINCPP "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp" CACHE STRING "Sets the entry point of the program") # Loads a diferent entry point for the program
option(DEV_MODE "Enable developing mode" OFF) # Enables some features like an initializing database method

# # Compilation CMake variables ##

# Determines the absolute path to header files to compile the program
set(HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include")

# # Installation / Uninstallation CMake variables ##

# Determines the absolute path to CMake installation / uninstallation scripts
set(SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake")

# Determines the installation path for library libinvmanl.so header files
set(INST_HEADERS_DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libinvmanl")

# Determines the installation path for the program executable
set(INST_RUNTIME_DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

# Determines the installation path for libinvmanl.so
set(INST_LIBRARY_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

# Determines the absolute path to the sql database initializer code
set(DB_INIT_FILE_DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/scripts/database/initializer.sql")

# Determines the absolute path to the directory in which the program stores data files
set(INST_ARCHIVES_DESTINATION "$ENV{HOME}/.local/share/invman")

# Determines where the database file is located
set(DB_DATA_FILE_DESTINATION "${INST_ARCHIVES_DESTINATION}/invman.db")

# Makes reference to the auto generated file install_manifest.txt path
set(INSTALL_MANIFEST "${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt")

# Determines the headers files manifest file location
set(HEADERS_MANIFEST "${CMAKE_CURRENT_BINARY_DIR}/headers_manifest.txt")

# Determines the archives (such as invman.db) manifest file location
set(ARCHIVES_MANIFEST "${CMAKE_CURRENT_BINARY_DIR}/archives_manifest.txt")

# # Includes the library compilation scripts ( sqlite3 and cxxopts ) ##

# It obtains and builds sqlite source files.
include("${SCRIPTS}/fetch-sqlite.cmake")

# It obtains cxxopts options parser library source
include("${SCRIPTS}/fetch-cxxopts.cmake")

# It creates the database file and a target which runs a python script that inits the database
include("${SCRIPTS}/create-initdb-target.cmake")

# It creates a interface lib for exposing headers to ${EXECUTABLE}
add_library(add_includes INTERFACE)
target_include_directories(add_includes INTERFACE ${HEADERS})

# Creates invmanl library target
file(GLOB DATABASE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/database/*.cpp")
file(GLOB PRODUCTS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/product/*.cpp")
file(GLOB UTILS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp")

add_library(invmanl SHARED ${DATABASE_SOURCE} ${PRODUCTS_SOURCE} ${UTILS_SOURCE})
target_link_libraries(invmanl PUBLIC add_includes)
target_link_libraries(invmanl PUBLIC cxxopts)
target_link_libraries(invmanl PUBLIC sqlite)

# Creates the executable target
add_executable(invman ${MAINCPP})

target_compile_definitions(invman PRIVATE "DB_DATA_FILE_DESTINATION=\"${DB_DATA_FILE_DESTINATION}\"")

# Executable target definitions
if(DEV_MODE)
  target_compile_definitions(invman PRIVATE "DEV_MODE")
endif()

# Dependencies
add_dependencies(invman init_database)

# Links the library with the executable
target_link_libraries(invman PRIVATE invmanl)

# Creates installation / desinstallation targets
include("${SCRIPTS}/create-install-target.cmake")
include("${SCRIPTS}/create-uninstall-target.cmake")