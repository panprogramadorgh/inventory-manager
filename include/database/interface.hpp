#ifndef DBINTERFACE_HPP
#define DBINTERFACE_HPP

#include "forwarder.hpp"
#include "product/product.hpp"
#include "sqlite3.h"
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

typedef std::pair<std::vector<std::string>, std::vector<std::string>>
    QueryResult;

typedef std::unordered_map<int, std::shared_ptr<Product>> QueryUmap;

class Database
{
private:
  sqlite3 *db;
  std::string file_name;
  std::vector<std::string> cols;
  std::vector<std::string> vals;

  void setDatabaseFile(std::string db_name);

  // Static methods

  static void checkFile(std::string f_name) noexcept(false);

public:
  Database() noexcept;

  Database(std::string f_name) noexcept(false);

  Database(const Database &other);

  Database(sqlite3 *other_db);

  std::string getDatabaseFile() const noexcept;

  void connect() noexcept(false);

  QueryResult fetchQuery() const noexcept;

  void executeQuery(std::string raw_query, std::vector<std::string> &&args = {}) const noexcept(false);

  void executeUpdate(std::string raw_query, std::vector<std::string> &&args = {}) const noexcept(false);

  ~Database()
  {
    sqlite3_close(db);
  }

  // Static methods

  static QueryUmap &umapQuery(QueryUmap &&dest, QueryResult &&qresult);

  static std::string mergeQueryArgs(std::string query, std::vector<std::string> &&args) noexcept;

  static void printQuery(const QueryResult qresult) noexcept;
  static void printQuery(const QueryUmap qumap) noexcept;
};

#endif