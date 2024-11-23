#include "../include/database/interface.hpp"
#include "../include/database/errors.hpp"

DatabaseError::DatabaseError(int code, std::string message)
    : code(code), message(message)
{
  this->message += ": ";
  this->message += std::to_string(code);
}

const char *DatabaseError::what() const noexcept
{
  return message.c_str();
}

ConnError::ConnError(int exit_code)
    : DatabaseError(exit_code, "Database connection error")
{
}

QueryError::QueryError(int query_result)
    : DatabaseError(query_result, "QueryError")
{
}

QueryError::QueryError(int query_result, char *query_message)
    : DatabaseError(query_result, "QueryError: " + std::string(query_message))
{
  sqlite3_free(query_message); // Se libera en caso de que el error de consulta provenga de la interfaz de sqlite3
}

InitError::InitError(int exit_code)
    : DatabaseError(exit_code, "Database initialization error")
{
}