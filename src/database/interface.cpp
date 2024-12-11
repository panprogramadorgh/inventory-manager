#include "database/interface.hpp"
#include "database/errors.hpp"
#include "product/product.hpp"
#include "utils/strutils.hpp"

// Non-static methods

void Database::executeQuery(std::string query, std::vector<std::string> &&args) const noexcept(false)
{
  std::string query = mergeQueryArgs(query, std::move(args)); // Monta la consulta con los argumentos indicados
  char *msg = nullptr;                                        // Mensaje de sliqte3 (debe ser liberado con sqlite3_free)
  int query_exit_code;                                        // Almacena el codigo de error de la consulta

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

void Database::executeUpdate(std::string query, std::vector<std::string> &&args) const noexcept(false)
{
  std::string query = mergeQueryArgs(query, std::move(args)); // Monta la consulta con los argumentos indicados
  char *msg = nullptr;                                        // Mensaje de sliqte3 (debe ser liberado con sqlite3_free)
  int query_exit_code;                                        // Almacena el codigo de error de la consulta

  query_exit_code = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &msg);
  if (query_exit_code != SQLITE_OK)
  {
    DatabaseError qerror(query_exit_code, msg);
    sqlite3_free(msg);
    throw qerror;
  }
}

// Static methods

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