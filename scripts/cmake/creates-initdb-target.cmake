# Configures the database initialization python script
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/init-db.py")
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/python/init-db.py.in"
    "${CMAKE_CURRENT_BINARY_DIR}/init-db.py"
  )
endif()

# Creates the database file
if(NOT EXISTS ${DB_DATA_FILE_DESTINATION})
  # Creates the database file
  file(WRITE ${DB_DATA_FILE_DESTINATION} "")

  # Writes the database file path at the archive manifest file
  file(WRITE ${ARCHIVES_MANIFEST} "${DB_DATA_FILE_DESTINATION}\n")
  message(STATUS "Created database file at ${DB_DATA_FILE_DESTINATION}")
else()
  message(STATUS "Database file was found")
endif()

# Creates init_database target to init databse
add_custom_target(init_database
  COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/init-db.py
)