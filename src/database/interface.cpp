#include "database/interface.hpp"
#include "database/dberror.hpp"
#include "product/product.hpp"

// Non-static methods

void Database::executeQuery(const std::string raw_query, std::vector<std::string> &&args) const noexcept(false)
{
  std::string query = mergeQueryArgs(raw_query, std::move(args)); // Monta la consulta con los argumentos indicados
  char *msg = nullptr;                                            // Mensaje de sliqte3 (debe ser liberado con sqlite3_free)
  int query_exit_code;                                            // Almacena el codigo de error de la consulta

  auto callback = [](void *ctx, int rows, char **vals, char **cols) -> int
  {
    Database *this_db = static_cast<Database *>(ctx);

    for (int i = 0; i < rows; i++)
    {
      bool col_exists = std::any_of(this_db->cols.begin(), this_db->cols.end(), [cols, i](const std::string &s)
                                    { return s == std::string(cols[i]); });
      // If the column does not exist, it is added to the query columns vector (i.e. it has not yet finished saving all the fields of the first record of the current query)
      if (!col_exists)
        this_db->cols.insert(this_db->cols.cend(), cols[i]);
      this_db->vals.insert(this_db->vals.cend(), vals[i]);
    }
    return 0;
  };

  query_exit_code = sqlite3_exec(db, query.c_str(), callback, (void *)this, &msg);
  if (query_exit_code != SQLITE_OK)
  {
    DatabaseError qerror(query_exit_code, msg);
    sqlite3_free(msg);
    throw qerror;
  }
}

void Database::executeUpdate(const std::string raw_query, std::vector<std::string> &&args) const noexcept(false)
{
  std::string query = mergeQueryArgs(raw_query, std::move(args)); // Monta la consulta con los argumentos indicados
  char *msg = nullptr;                                            // Mensaje de sliqte3 (debe ser liberado con sqlite3_free)
  int query_exit_code;                                            // Almacena el codigo de error de la consulta

  query_exit_code = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &msg);
  if (query_exit_code != SQLITE_OK)
  {
    DatabaseError qerror(query_exit_code, msg);
    sqlite3_free(msg);
    throw qerror;
  }
}

// Static methods

void Database::printQuery(const QueryResult qresult) noexcept
{
  auto cols = qresult.first;
  auto vals = qresult.second;

  for (auto it = cols.cbegin(); it != cols.cend(); it++)
    std::cout << *it << (it == cols.cend() - 1 ? "\n" : ",");

  for (auto it = vals.cbegin(); it != vals.cend(); it++)
    std::cout << *it << (std::distance(vals.cbegin(), it) % cols.size() == cols.size() - 1 ? "\n" : ",");
}