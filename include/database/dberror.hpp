#ifndef DBERRORS_HPP
#define DBERRORS_HPP

#include "database/dbinterface.hpp"
#include <cstring>

/* Database errors class */
class DatabaseError : public std::exception
{
protected:
  std::string error_message;
  int code;

public:
  // Generic error messages
  struct ErrMsgs
  {
    static constexpr char OPENING_FAILED[] = "Database opening failed";
    static constexpr char INITIALIZATION_FAILED[] = "Database initialization failed";
    static constexpr char DB_IS_NOT_OPEN[] = "Database not open";
  };

  // Constructors
  DatabaseError(const int code, const char *error_desc = nullptr) : code(code)
  {
    error_message += "Database error (" + std::to_string(code) + ")";
    if (error_desc)
    {
      error_message += " : ";
      error_message += error_desc;
    }
  }

  DatabaseError(DatabaseError &other)
      : code(other.code), error_message(other.error_message)
  {
  }

  DatabaseError(DatabaseError &&other)
      : code(other.code), error_message(std::move(other.error_message))
  {
    other.code = -1;
  }

  // Methods

  const char *what() const noexcept
  {
    return error_message.c_str();
  }

  int getErrorCode()
  {
    return code;
  }

  // Operators

  operator bool()
  {
    return code > -1;
  }

  DatabaseError &operator=(DatabaseError &other)
  {
    if (this != &other)
    {
      code = other.code;
      error_message = other.error_message;
    }
    return *this;
  }

  DatabaseError &operator=(DatabaseError &&other)
  {
    if (this != &other)
    {
      code = other.code;
      other.code = -1;
      error_message = std::move(other.error_message);
    }
    return *this;
  }

  ~DatabaseError()
  {
    code = -1;
  }
};
#endif