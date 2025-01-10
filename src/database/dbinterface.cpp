#include "database/dbinterface.hpp"

// Non-static methods

void Database::connect()
{
  int exit_code = std::make_error_code(std::errc::operation_not_supported).value();

  if (!should_open || (exit_code = sqlite3_open(db_file.c_str(), &db)))
    throw DatabaseError(exit_code, DatabaseError::ErrMsgs::OPENING_FAILED);

  should_open = false;
}

void Database::executeQuery(const std::string sql) const
{
  if (should_open)
    throw DatabaseError(EPERM, DatabaseError::ErrMsgs::DB_IS_NOT_OPEN);

  char *msg = nullptr; // Mensaje de sliqte3 (debe ser liberado con sqlite3_free)
  int query_exit_code; // Almacena el codigo de error de la consulta

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

  query_exit_code = sqlite3_exec(db, sql, callback, (void *)this, &msg);
  if (query_exit_code != SQLITE_OK)
  {
    DatabaseError qerror(query_exit_code, msg);
    sqlite3_free(msg);
    throw qerror;
  }
}

void Database::executeUpdate(const std::string sql) const
{
  if (should_open)
    throw DatabaseError(EPERM, DatabaseError::ErrMsgs::DB_IS_NOT_OPEN);

  char *msg = nullptr; // Mensaje de sliqte3 (debe ser liberado con sqlite3_free)
  int query_exit_code; // Almacena el codigo de error de la consulta

  query_exit_code = sqlite3_exec(db, sql, nullptr, nullptr, &msg);
  if (query_exit_code != SQLITE_OK)
  {
    DatabaseError qerror(query_exit_code, msg);
    sqlite3_free(msg);
    throw qerror;
  }
}