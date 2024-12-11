# Obtaining cxxopts source code
set(CXXOPTS_VERSION "3.2.0")
set(CXXOPTS_URL "https://github.com/jarro2783/cxxopts.git")
set(CXXOPTS_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/cxxopts")
set(CXXOPTS_REPO_DIR "${CMAKE_CURRENT_BINARY_DIR}/cxxopts/cxxopts-repo")
set(CXXOPTS_CMAKE_FILE "${CXXOPTS_SOURCE_DIR}/CMakeLists.txt")

# Clones the cxxtops repository
if(NOT EXISTS ${CXXOPTS_SOURCE_DIR})
  make_directory(${CXXOPTS_SOURCE_DIR})
  execute_process(
    COMMAND git clone -b v${CXXOPTS_VERSION} --single-branch ${CXXOPTS_URL} ${CXXOPTS_REPO_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  execute_process(
    COMMAND cp "${CXXOPTS_REPO_DIR}/include/cxxopts.hpp" ${CXXOPTS_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  execute_process(
    COMMAND rm -rf ${CXXOPTS_REPO_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  message(STATUS "cxxopts-${CXXOPTS_VERSION} repo was cloned at ${CXXOPTS_REPO_DIR}")
elseif()
  message(STATUS "cxxopts-${CXXOPTS_VERSION} was found at ${CXXOPTS_REPO_DIR}")
endif()

if(NOT EXISTS ${CXXOPTS_CMAKE_FILE})
  file(WRITE ${CXXOPTS_CMAKE_FILE} "
cmake_minimum_required(VERSION 3.10)
project(CXXOPTS)

add_library(cxxopts INTERFACE)
target_include_directories(cxxopts INTERFACE ${CXXOPTS_SOURCE_DIR})
")
endif()

add_subdirectory(${CXXOPTS_SOURCE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/cxxopts_build")