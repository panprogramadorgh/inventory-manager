#include <cstring>
#include "../include/database/interface.hpp"
#include "../include/database/errors.hpp"

/* Base class for database errors */

DatabaseError::DatabaseError(const int code, const char *msg)
    : code(code)
{
  const char *default_msg = "Database error";
  std::size_t msg_len = std::strlen(msg != nullptr ? msg : default_msg);
  message = new char[msg_len];
  std::strcpy(message, msg != nullptr ? msg : default_msg);
}

const char *DatabaseError::what() const noexcept
{
  std::string out = std::string(message) + ": " + std::to_string(code);
  return out.c_str();
}

/* The following initialize the super class with the specific message */

ConnError::ConnError(const int conn_exit_code)
    : DatabaseError(conn_exit_code, "Database connection error")
{
}

QueryError::QueryError(const int query_exit_code)
    : DatabaseError(query_exit_code, "Query failure"), query_err_message(nullptr)
{
}

QueryError::QueryError(const int query_exit_code, const char *msg)
    : DatabaseError(query_exit_code, "Query failure"), query_err_message(nullptr)
{
  std::size_t msg_len = std::strlen(msg);
  query_err_message = new char[msg_len];
  std::strcpy(query_err_message, msg);
}

const char *QueryError::what() const noexcept
{
  std::string output;
  if (query_err_message != nullptr)
  {
    output += std::string(message) + ":\n";
    output += "\t" + std::string(query_err_message) + "\n";
  }
  else
  {
    output += message;
  }
  return output.c_str();
}

InitError::InitError(int exit_code)
    : DatabaseError(exit_code, "Database initialization error")
{
}