configure_file(
  "${SCRIPTS}/install-headers.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/install-headers.cmake"
  @ONLY
)

# Creates the install target in which invman and libinvmanl is moved
install(
  TARGETS invmanl invman
  RUNTIME DESTINATION ${INST_RUNTIME_DESTINATION}
  LIBRARY DESTINATION ${INST_LIBRARY_DESTINATION}
)

# Creates the headers destination path directory
install(
  DIRECTORY DESTINATION
  ${INST_HEADERS_DESTINATION}
)

# Calls the install-headers script that copies the header files
install(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/install-headers.cmake")