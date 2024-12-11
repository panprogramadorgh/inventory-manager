# Creates the sqlite database file at $HOME

execute_process(
  COMMAND printenv HOME
  OUTPUT_VARIABLE HOME_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(DATABASE_FILE "${HOME_DIR}/inventory-manager.db")

if(NOT EXISTS ${DATABASE_FILE})
  add_custom_command(
    OUTPUT ${DATABASE_FILE}
    COMMAND touch ${DATABASE_FILE}
    COMMENT "Database file was created at ${DATABASE_FILE}"
  )
else()
  message(STATUS "Database file was found at ${DATABASE_FILE}")
endif()

add_custom_target(create_database_file
  DEPENDS ${DATABASE_FILE}
)