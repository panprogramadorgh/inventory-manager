#ifndef DBINTERFACE_HPP
#define DBINTERFACE_HPP

#include "forwarder.hpp"
#include "product/product.hpp"
#include "database/dberror.hpp"
#include "utils/strutils.hpp"
#include "sqlite3.h"
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

using QueryResult = std::pair<std::vector<std::string>, std::vector<std::string>>;

template <IsPrInfoBased T>
using QueryUmap = umap<int, std::shared_ptr<T>>;

class Database
{
private:
  sqlite3 *db;
  const std::string file_name;
  std::vector<std::string> cols;
  std::vector<std::string> vals;
  bool should_close_db, should_open_db;

  // Static methods
  static void checkFile(const std::string fname)
  {
    if (std::filesystem::exists(fname))
      return;
    std::error_code ec = std::make_error_code(std::errc::no_such_file_or_directory);
    fs::path path(fname);
    throw fs::filesystem_error(std::string("Could not find database file: ") + path.string(), path, ec);
  }

public:
  Database(const std::string fname)
      : db(nullptr), file_name(fname), should_open_db(true), should_close_db(true)
  {
    checkFile(fname);
  }

  Database(Database &&other)
      : should_open_db(other.should_open_db),
        should_close_db(other.should_close_db),
        db(other.db),
        cols(std::move(other.cols)),
        vals(std::move(other.vals))
  {
    other.should_open_db = false;
    other.should_close_db = false;
    other.db = nullptr;
  }

  // Allows the posibility of extern management of sqlite3 connection
  Database(sqlite3 *other_db, bool should_open_db, bool should_close_db)
      : file_name("Reused sqlite3 struct"), db(other_db), should_close_db(should_close_db), should_open_db(should_open_db)
  {
  }

  // Opens database file. If database should not be open or there is an sqlite3 failure, it throws an instance of DatabaseError
  void connect()
  {
    int exit_code;
    if (!should_open_db || (exit_code = sqlite3_open(file_name.c_str(), &db)))
      throw DatabaseError(exit_code, DatabaseError::GenericErrMsgs::OPENING_FAILED);
  }

  // Database interaction utilities
  void executeQuery(std::string query, std::vector<std::string> &&args = {}) const noexcept(false);
  void executeUpdate(std::string query, std::vector<std::string> &&args = {}) const noexcept(false);

  // Exposes the columns and values ​​resulting from the query
  QueryResult fetchQuery() const noexcept
  {
    return std::make_pair(cols, vals);
  }

  ~Database()
  {
    if (should_close_db)
      sqlite3_close(db);
  }

  // Static methods

  // Allows you to format SQL queries with a vector of arguments
  static std::string mergeQueryArgs(std::string query, const std::vector<std::string> &&args) noexcept
  {
    std::size_t pos, i = 0;

    while ((pos = query.find('$')) != std::string::npos)
      strInsert(query, args.at(i++), pos, 1);

    return query;
  }

  // FIXME: Arreglar delcaracion con definicion
  template <typename T>
  static void printQuery(const QueryUmap<T> qresult) noexcept;
  static void printQuery(const QueryResult qresult) noexcept;
};

#endif