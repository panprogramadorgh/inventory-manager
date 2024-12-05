#ifndef DBERRORS_HPP
#define DBERRORS_HPP

#include "../database/interface.hpp"

/* Base class for database errors */
class DatabaseError : public std::exception
{
protected:
  char *message;
  const int code;

public:
  DatabaseError(const int code, const char *msg);
  virtual const char *what() const noexcept;

  inline std::string toString() const noexcept
  {
    return message;
  }

  ~DatabaseError()
  {
    delete[] message;
  }
};

/* The following initialize the super class with the specific message */

class ConnError : public DatabaseError
{
public:
  ConnError(const int conn_exit_code);
};

class QueryError : public DatabaseError
{
public:
  char *query_err_message;

  QueryError(const int query_exit_code);
  QueryError(const int query_exit_code, const char *msg);

  const char *what() const noexcept override;
};

class InitError : public DatabaseError
{
public:
  InitError(int init_exit_code);
};

#endif