# Obtaining sqlite source files
set(SQLITE_VERSION "3.47.1")
set(SQLITE_URL "https://www.sqlite.org/2024/sqlite-amalgamation-3470000.zip")
set(SQLITE_ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/sqlite-${SQLITE_VERSION}.zip")
set(SQLITE_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/sqlite-${SQLITE_VERSION}")
set(SQLITE_AMAL "${SQLITE_SOURCE_DIR}/sqlite-amalgamation-3470000")
set(SQLITE_CMAKE_FILE "${SQLITE_SOURCE_DIR}/CMakeLists.txt")

if(NOT EXISTS ${SQLITE_ARCHIVE})
  message(STATUS "Obtaining sqlite archive from ${SQLITE_URL}")
  file(DOWNLOAD ${SQLITE_URL} ${SQLITE_ARCHIVE} SHOW_PROGRESS)
endif()

if(NOT EXISTS ${SQLITE_SOURCE_DIR})
  if(NOT EXISTS ${SQLITE_AMAL})
  message(STATUS "Extracting sqlite archive ${SQLITE_ARCHIVE} at ${SQLITE_AMAL}")
    execute_process(
      COMMAND unzip ${SQLITE_ARCHIVE} -d ${SQLITE_SOURCE_DIR}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
  endif()
  execute_process(
    COMMAND cp "${SQLITE_AMAL}/sqlite3.c" ${SQLITE_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  execute_process(
    COMMAND cp "${SQLITE_AMAL}/sqlite3.h" ${SQLITE_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()

if(NOT EXISTS ${SQLITE_CMAKE_FILE})
  file(WRITE ${SQLITE_CMAKE_FILE} "
    cmake_minimum_required(VERSION 3.10)
    project(Sqlite)
    add_library(sqlite STATIC \"${SQLITE_SOURCE_DIR}/sqlite3.c\")
    target_include_directories(sqlite PUBLIC \"${SQLITE_SOURCE_DIR}\")
  ")
  message(STATUS "Generating dynamically ${SQLITE_CMAKE_FILE}")
endif()

add_subdirectory(${SQLITE_SOURCE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/sqlite_build")