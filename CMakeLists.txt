cmake_minimum_required(VERSION 3.10)

project(InventoryManager)

file(GLOB DATABASE_SOURCE "src/database/*.cpp")
file(GLOB PRODUCTS_SOURCE "src/product/*.cpp")
file(GLOB UTILS_SOURCE "src/utils/*.cpp")
set(SQLITE_SOURCE "src/lib/sqlite/sqlite3.c")

add_library(invemanager SHARED ${DATABASE_SOURCE} ${PRODUCTS_SOURCE} ${UTILS_SOURCE})
add_library(sqlite SHARED ${SQLITE_SOURCE})

target_include_directories(invemanager PUBLIC src/include)
target_include_directories(sqlite PUBLIC src/include/sqlite)

target_link_libraries(invemanager PUBLIC sqlite)

add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE invemanager sqlite)