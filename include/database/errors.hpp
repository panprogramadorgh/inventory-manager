#ifndef DBERRORS_HPP
#define DBERRORS_HPP

#include "database/interface.hpp"
#include <cstring>

/* Base class for database errors */
class DatabaseError : public std::exception
{
protected:
  char *error_message;
  int code;

public:
  struct ErrorMessages
  {
    static constexpr char OPENING_FAILED[] = "Database opening failed";
    static constexpr char INITIALIZATION_FAILED[] = "Database initialization failed";
  };

  // Constructors
  DatabaseError(const int code = -1, const char *msg = nullptr) : code(code)
  {
    if (msg == nullptr)
      msg = "Database error";
    std::size_t msg_len = std::strlen(msg);
    error_message = new char[msg_len + 1];
    std::strcpy(error_message, msg);
  }

  DatabaseError(DatabaseError &other)
      : code(other.code), error_message(nullptr)
  {
    if (other.error_message != nullptr)
    {
      error_message = new char[std::strlen(other.error_message) + 1];
      std::strcpy(error_message, other.error_message);
    }
  }

  DatabaseError(DatabaseError &&other)
      : code(other.code), error_message(nullptr)
  {
    if (other.error_message != nullptr)
    {
      error_message = new char[std::strlen(other.error_message) + 1];
      std::strcpy(error_message, other.error_message);
      delete[] other.error_message;
      other.error_message = nullptr;
    }
    other.code = -1;
  }

  // Methods

  const char *what() const noexcept
  {
    return error_message;
  }

  int getErrorCode()
  {
    return code;
  }

  // Operators

  operator bool()
  {
    return error_message;
  }

  DatabaseError &operator=(DatabaseError &other)
  {
    if (this != &other)
    {
      if (error_message != nullptr)
        delete[] error_message; // Dispatches old mem block;
      code = other.code;
      if (other.error_message == nullptr)
        error_message = nullptr;
      else
      {
        error_message = new char[std::strlen(other.error_message) + 1];
        std::strcpy(error_message, other.error_message);
      }
    }
    return *this;
  }

  DatabaseError &operator=(DatabaseError &&other)
  {
    if (this != &other)
    {
      if (error_message != nullptr)
        delete[] error_message; // Dispatches old mem block;
      code = other.code;
      if (other.error_message == nullptr)
        error_message = nullptr;
      else
      {
        error_message = new char[std::strlen(other.error_message) + 1];
        std::strcpy(error_message, other.error_message);
        delete[] other.error_message;
        other.error_message = nullptr;
      }
    }
    return *this;
  }

  ~DatabaseError()
  {
    if (error_message)
      delete[] error_message;
  }
};
#endif