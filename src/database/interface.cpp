#include "database/interface.hpp"
#include "database/errors.hpp"
#include "product/product.hpp"
#include "utils/strutils.hpp"

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

QueryUmap &Database::umapQuery(QueryUmap &&dest, QueryResult &&qresult)
{
  UmappedProduct row;
  ProductField field;

  auto cols = qresult.first;
  auto vals = qresult.second;

  for (auto it = vals.begin();
       it != vals.cend() &&
       std::distance(vals.begin(), it) % cols.size() < cols.size();
       it++)
  {
    field = string_to_product_field.at(cols[std::distance(vals.begin(), it) % cols.size()]);
    row[field] = *it;

    // En caso de que sea la ulima columna por registro
    if (std::distance(vals.begin(), it) % cols.size() == cols.size() - 1)
    {
      // Creates a non-virtual ProductInfo
      ProductInfo pinfo(row, false);
      dest.emplace(pinfo.product_id, std::make_shared<Product>(Product(pinfo)));
      row.clear(); // Limpiar el mapa para la proxima fila
    }
  }

  return dest;
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
    throw DatabaseError(exit_code, DatabaseError::connection_error_message);
}

QueryResult Database::fetchQuery() const noexcept
{
  return std::make_pair(cols, vals);
}

void Database::executeQuery(std::string raw_query, std::vector<std::string> &&args) const noexcept(false)
{
  std::string query = mergeQueryArgs(raw_query, std::move(args)); // Monta la consulta con los argumentos indicados
  char *msg = nullptr;                                            // Mensaje de sliqte3 (debe ser liberado con sqlite3_free)
  int query_exit_code;                                            // Almacena el codigo de error de la consulta

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

  query_exit_code = sqlite3_exec(db, query.c_str(), callback, (void *)this, &msg);
  if (query_exit_code != SQLITE_OK)
  {
    DatabaseError qerror(query_exit_code, msg);
    sqlite3_free(msg);
    throw qerror;
  }
}

void Database::executeUpdate(std::string raw_query, std::vector<std::string> &&args) const noexcept(false)
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

void Database::setDatabaseFile(std::string db_name)
{
  file_name = db_name;
}

/* Static methods */

void Database::printQuery(const QueryResult qresult) noexcept
{
  auto cols = qresult.first;
  auto vals = qresult.second;

  for (auto it = cols.cbegin(); it != cols.cend(); it++)
    std::cout << *it << (it == cols.cend() - 1 ? "\n" : ",");

  for (auto it = vals.cbegin(); it != vals.cend(); it++)
    std::cout << *it << (std::distance(vals.cbegin(), it) % cols.size() == cols.size() - 1 ? "\n" : ",");
}

void Database::printQuery(const QueryUmap qumap) noexcept
{
  ProductInfo pinfo; // Intermediate step to umap product
  UmappedProduct up;

  for (auto fit = product_field_to_string.cbegin(); fit != product_field_to_string.cend(); fit++)
  {
    if (fit != product_field_to_string.cbegin())
      std::cout << ',';
    std::cout << fit->second;
  }
  for (auto pit = qumap.cbegin(); pit != qumap.cend(); pit++)
  {
    pinfo = pit->second->info();
    up = Product::umapProduct(pinfo);
    for (auto fit = up.cbegin(); fit != up.cend(); fit++)
    {
      if (fit != up.cbegin())
        std::cout << ',';
      std::cout << fit->second;
    }
    std::cout << std::endl;
  }
}

std::string Database::mergeQueryArgs(std::string query, std::vector<std::string> &&args) noexcept
{
  std::size_t pos, i = 0;

  while ((pos = query.find('$')) != std::string::npos)
    strInsert(query, args.at(i++), pos, 1);

  return query;
}