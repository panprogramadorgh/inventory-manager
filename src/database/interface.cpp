#include "../include/database/interface.hpp"
#include "../include/database/errors.hpp"
#include "../include/product/product.hpp"
#include "../include/utils/strutils.hpp"

#include <memory> // FIXME: Debug

// Static methods
void Database::checkFile(std::string f_name) noexcept(false)
{
  if (!std::filesystem::exists(f_name))
  {
    std::error_code ec = std::make_error_code(std::errc::no_such_file_or_directory);

    std::filesystem::path file_path = f_name;
    std::string message = "Could not find database file: " + file_path.string();

    // Lanza error de archivo no encontrado
    throw std::filesystem::filesystem_error(message, file_path, ec);
  }
}

std::unordered_map<int, std::shared_ptr<Product>> Database::parseQueryToUmap(std::unordered_map<int, std::shared_ptr<Product>> &&dest, QueryResult &&qresult)
{
  std::unordered_map<std::string, std::string> row;

  auto cols = qresult.first;
  auto vals = qresult.second;

  for (auto it = vals.begin();
       it != vals.cend() &&
       std::distance(vals.begin(), it) % cols.size() < cols.size();
       it++)
  {
    row[cols[std::distance(vals.begin(), it) % cols.size()]] = *it;

    // En caso de que sea la ulima columna por registro
    if (std::distance(vals.begin(), it) % cols.size() == cols.size() - 1)
    {
      Product p(row); // It creates a full product with id member
      dest.emplace(p.identifier(), std::make_shared<Product>(p));
      row.clear(); // Limpiar el mapa para la proxima fila
    }
  }

  return dest;
}

void Database::printQuery(QueryResult qresult) noexcept
{
  auto cols = qresult.first;
  auto vals = qresult.second;

  for (auto it = cols.cbegin(); it != cols.cend(); it++)
    std::cout << *it << (it == cols.cend() - 1 ? "\n" : ",");

  for (auto it = vals.cbegin(); it != vals.cend(); it++)
    std::cout << *it << (std::distance(vals.cbegin(), it) % cols.size() == cols.size() - 1 ? "\n" : ",");
}

std::string &Database::mergeQueryArgs(std::string &&query, std::vector<std::string> &&args) noexcept
{
  std::size_t pos, i = 0;

  while ((pos = query.find('$')) != std::string::npos)
    strInsert(query, args.at(i++), pos, 1);

  return query;
}

Database::Database() noexcept
    : db(nullptr)
{
  // Se deben inicializar debidamente los miembros
}

Database::Database(std::string f_name) noexcept(false)
    : file_name(f_name)
{
  checkFile(f_name);
}

Database::Database(const Database &other)
{
  setDatabaseFile(other.getDatabaseFile());
  cols = other.cols;
  vals = other.vals;
}

Database::Database(sqlite3 *other_db)
{
  sqlite3_close(db);
  db = other_db;
}

std::string Database::getDatabaseFile() const noexcept
{
  return this->file_name;
}

void Database::connect() noexcept(false)
{
  int exit_code;
  if ((exit_code = sqlite3_open(file_name.c_str(), &db)))
    throw ConnError(exit_code);
}

QueryResult Database::fetchQuery() const noexcept
{
  return std::make_pair(cols, vals);
}

void Database::executeQuery(std::string raw_query, std::vector<std::string> args) const noexcept(false)
{
  std::string query = mergeQueryArgs(std::move(raw_query), std::move(args)); // Monta la consulta con los argumentos indicados
  char *query_msg = nullptr;                                                 // Mensaje de sliqte3 (debe ser liberado con sqlite3_free)
  int query_result;                                                          // Almacena el codigo de error de la consulta

  // Escribir registros en la pantalla
  auto callback = [](void *ctx, int rows, char **vals, char **cols) -> int
  {
    Database *this_db = static_cast<Database *>(ctx);

    for (int i = 0; i < rows; i++)
    {
      bool col_exists = std::any_of(this_db->cols.begin(), this_db->cols.end(), [cols, i](const std::string &s)
                                    { return s == std::string(cols[i]); });
      // Si no existe la columna, es añadida al vector de columnas de consulta (es decir, todavia no ha terminado de guardar todos los campos del primer registro de la query actual)
      if (!col_exists)
        this_db->cols.insert(this_db->cols.cend(), cols[i]);
      this_db->vals.insert(this_db->vals.cend(), vals[i]);
    }
    return 0;
  };

  // Callback tiene tipo incompatible con sqlite3_exec
  query_result = sqlite3_exec(db, query.c_str(), callback, (void *)this, &query_msg);

  if (query_result != SQLITE_OK)
  {
    if (query_msg != nullptr)
      throw QueryError(query_result, query_msg);
    else
      throw QueryError(query_result);
  };
}

void Database::executeUpdate(std::string raw_query, std::vector<std::string> args) const noexcept(false)
{
  std::string query = mergeQueryArgs(std::move(raw_query), std::move(args)); // Monta la consulta con los argumentos indicados
  char *query_msg = nullptr;                                                 // Mensaje de sliqte3 (debe ser liberado con sqlite3_free)
  int query_result;                                                          // Almacena el codigo de error de la consultaF

  query_result = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &query_msg);
  if (query_result != SQLITE_OK)
    throw QueryError(query_result, query_msg);
}

Database::~Database()
{
  sqlite3_close(db);
}

void Database::setDatabaseFile(std::string db_name)
{
  file_name = db_name;
}