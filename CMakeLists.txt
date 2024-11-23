cmake_minimum_required(VERSION 3.10)

project(InventoryManager)

file(GLOB DATABASE_SOURCE "src/database/*.cpp")
file(GLOB PRODUCTS_SOURCE "src/product/*.cpp")
file(GLOB UTILS_SOURCE "src/utils/*.cpp")

add_library(database SHARED ${DATABASE_SOURCE})
add_library(products SHARED ${PRODUCTS_SOURCE})
add_library(utils SHARED ${UTILS_SOURCE})
add_library(sqlite SHARED src/lib/sqlite/sqlite3.c)

target_include_directories(database PUBLIC src/include)
target_include_directories(products PUBLIC src/include)
target_include_directories(utils PUBLIC src/include)
target_include_directories(sqlite PUBLIC src/include/sqlite)

add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE database products utils sqlite)