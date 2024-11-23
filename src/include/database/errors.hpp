#ifndef DBERRORS_HPP
#define DBERRORS_HPP

#include "../database/interface.hpp"

class DatabaseError : public std::exception
{
private:
  std::string message;
  int code;

public:
  DatabaseError(int code, std::string message);

  virtual const char *what() const noexcept;
};

class ConnError : public DatabaseError
{
public:
  ConnError(int exit_code);
};

class QueryError : public DatabaseError
{
public:
  QueryError(int query_result);

  QueryError(int query_result, char *query_message);
};

class InitError : public DatabaseError
{
public:
  InitError(int exit_code);
};

#endif