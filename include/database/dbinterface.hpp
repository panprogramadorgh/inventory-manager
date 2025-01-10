#ifndef DBINTERFACE_HPP
#define DBINTERFACE_HPP

#include "forwarder.hpp"
#include "database/dberror.hpp"
#include "utils/strutils.hpp"
#include <filesystem>
#include <functional>
#include <system_error>
#include <memory>
#include "sqlite3.h"

namespace fs = std::filesystem;

using QueryResult = std::pair<vec<std::string>, vec<std::string>>;

class Database
{
private:
  std::unique_ptr<sqlite3> db;
  bool should_open;
  std::string db_file;

  // Stores query results with fetchQuery method
  vec<std::string> cols;
  vec<std::string> vals;

public:
  Database(const std::string file_path)
      : db(nullptr), should_open(true), db_file(file_path)
  {
    if (!fs::exists(filename))
      std::invalid_argument(std::string("Invalid argument for filename : ") + std::error_code(ENOENT, std::system_category);
  }

  Database(const Database &other) = delete;

  Database(Database &&other)
      : db(std::move(other.db)),
        should_open(other.should_open),
        db_file(std::move(other.db_file)),
        cols(std::move(other.cols)),
        vals(std::move(other.vals))
  {
    other.should_open = false;
  }

  // Allows the posibility of extern management of sqlite3 connection
  Database(sqlite3 &other_db, const bool is_open = false)
      : db(other_db), should_open(!is_open)
  {
  }

  /// @brief Opens database file. If database should not be open or there is an sqlite3 failure, it throws an instance of DatabaseError with "operation_not_supported" error code since it should not be possible to open the database
  void connect();

  // Database interaction utilities
  void executeQuery(const std::string sql, const vec<std::string> &args = {}) const;
  void executeUpdate(const std::string sql, const vec<std::string> &args = {}) const;

  // Exposes the columns and values ​​resulting from the query
  QueryResult fetchQuery() const noexcept
  {
    return std::make_pair(cols, vals);
  }

  // Operators

  bool operator bool()
  {
    return (db != nullptr) && !should_open;
  }

  Database &operator=(const Database &other) = delete;

  Database &operator=(Database &&other)
  {
    if (&other != this)
    {
      db = std::move(other.db);
      should_open = other.should_open;
      db_file = std::move(other.db_file);
      cols = std::move(other.cols);
      vals = std::move(other.vals);

      other.should_open = false;
    }
    return *this;
  }

  virtual ~Database() = default;
};

#endif