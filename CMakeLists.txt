cmake_minimum_required(VERSION 3.10)
project(InventoryManager)

# It obtains and builds sqlite source files.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts/fetch-sqlite.cmake")

# It creates the sqlite database file
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts/create-database.cmake")

# It obtains cxxopts options parser library source
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts/fetch-cxxopts.cmake")


file(GLOB DATABASE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/database/*.cpp")
file(GLOB PRODUCTS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/product/*.cpp")
file(GLOB UTILS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp")
add_executable(main "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp" ${DATABASE_SOURCE} ${PRODUCTS_SOURCE} ${UTILS_SOURCE})

target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include/database")
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include/product")
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include/utils")

add_dependencies(main create_database_file)
target_link_libraries(main PRIVATE cxxopts)
target_link_libraries(main PRIVATE sqlite)