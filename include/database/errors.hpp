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
  // Constructors
  DatabaseError(const int code, const char *msg = nullptr)
      : code(code)
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

  virtual DatabaseError &operator=(DatabaseError &other)
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

  virtual DatabaseError &operator=(DatabaseError &&other)
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

  // Methods

  const char *what() const noexcept
  {
    std::string out = std::string(error_message) + ": " + std::to_string(code);
    return out.c_str();
  }

  inline std::string toString() const noexcept
  {
    return error_message;
  }

  virtual ~DatabaseError()
  {
    if (error_message)
      delete[] error_message;
  }
};

/* The following initialize the super class with the specific message */

class QueryError : public DatabaseError
{
public:
  char *query_err_message;

  QueryError(const int query_exit_code, const char *msg = nullptr)
      : DatabaseError(query_exit_code, "Query failure"),
        query_err_message(nullptr)
  {
    if (msg != nullptr)
    {
      std::size_t msg_len = std::strlen(msg);
      query_err_message = new char[msg_len + 1];
      std::strcpy(query_err_message, msg);
    }
  }

  QueryError(QueryError &other)
      : DatabaseError(other), query_err_message(nullptr)
  {
    if (other.query_err_message != nullptr)
    {
      query_err_message = new char[std::strlen(other.query_err_message) + 1];
      std::strcpy(query_err_message, other.query_err_message);
    }
  }

  QueryError(QueryError &&other)
      : DatabaseError(other), query_err_message(nullptr)
  {
    if (other.query_err_message != nullptr)
    {
      query_err_message = new char[std::strlen(other.query_err_message) + 1];
      std::strcpy(query_err_message, other.query_err_message);
      delete[] other.query_err_message;
      other.query_err_message = nullptr;
    }
  }

  const char *what() const noexcept
  {
    std::string output;
    if (query_err_message != nullptr)
    {
      output += std::string(error_message) + ":\n";
      output += "\t" + std::string(query_err_message) + "\n";
    }
    else
      output += error_message;
    return output.c_str();
  }

  // Operators

  QueryError &operator=(QueryError &other)
  {
    if (this != &other)
    {
      if (query_err_message != nullptr)
        delete[] query_err_message; // Dispatches old mem block;
      if (other.query_err_message == nullptr)
        query_err_message = nullptr;
      else
      {
        query_err_message = new char[std::strlen(other.query_err_message) + 1];
        std::strcpy(query_err_message, other.query_err_message);
      }
    }
    return *this;
  }

  QueryError &operator=(QueryError &&other)
  {
    if (this != &other)
    {
      if (query_err_message != nullptr)
        delete[] query_err_message; // Dispatches old mem block;
      if (other.query_err_message == nullptr)
        query_err_message = nullptr;
      else
      {
        query_err_message = new char[std::strlen(other.query_err_message) + 1];
        std::strcpy(query_err_message, other.query_err_message);
        delete[] other.query_err_message;
        other.query_err_message = nullptr;
      }
    }
    return *this;
  }

  ~QueryError()
  {
    if (query_err_message != nullptr)
      delete[] query_err_message;
  }
};

class ConnError : public DatabaseError
{
public:
  ConnError(const int conn_exit_code)
      : DatabaseError(conn_exit_code, "Database connection error")
  {
  }
};

class InitError : public DatabaseError
{
public:
  InitError(int exit_code)
      : DatabaseError(exit_code, "Database initialization error")
  {
  }
};

#endif