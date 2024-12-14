if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  message(FATAL_ERROR "CMAKE_INSTALL_PREFIX is not defined")
endif()

# Configures the template for uninstall.cmake script
configure_file(
  "${SCRIPTS}/uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
  @ONLY
)

# Creates uninstall target which runs uninstall.cmake script
add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
)