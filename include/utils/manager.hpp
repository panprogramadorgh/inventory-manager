#ifndef INTERFACE_MANAGER_HPP
#define INTERFACE_MANAGER_HPP

#include "forwarder.hpp"
#include "database/interface.hpp"
#include <optional>

template <typename T>
class Manager
{
public:
  // Types
  using Container = umap<std::size_t, std::shared_ptr<T>>;

  template <typename U = T>
  using SecureReturn = std::pair<std::optional<U>, std::string>;

protected:
  Database *db;
  bool should_free_db;

  Container cache;

public:
  struct ErrMsgs
  {
    static constexpr char DB_IS_NULL[] = "Database is null";
  };

  Manager(std::string dbfile)
      : db(nullptr), should_free_db(true)
  {
    db = new Database(dbfile);
    db->connect();
    ;
  }

  // Must receive a connected database (Database::connect())
  Manager(Database *db, bool should_free_db = false)
      : db(db), should_free_db(should_free_db)
  {
    if (!db)
      throw std::runtime_error(ErrMsgs::DB_IS_NULL);
  }

  // other.db must be already connected (Database::connect())
  Manager(Manager<T> &&other)
      : db(other.db),
        should_free_db(other.should_free_db),
        cache(std::move(other.cache))
  {
    other.db = nullptr;
  }

  ~Manager()
  {
    if (should_free_db)
      delete db;
    db = nullptr;
  }

  // Methods

  virtual std::size_t addCache(std::shared_ptr<T>) noexcept = 0;

  virtual std::size_t remCache(const std::size_t) noexcept = 0;

  // Static methods

  // TODO: Arreglar declaracion umapQuery

  static std::enable_if_t<std::is_same_v<T, ProductInfo>, Container> umapQuery(QueryResult qresult)
  {
    Container dest;
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
        dest.emplace(pinfo.product_id, std::make_shared<ProductInfo>(pinfo));
        row.clear(); // Limpiar el mapa para la proxima fila
      }
    }
    return dest;
  }

  static std::enable_if_t<std::is_same_v<T, Product>, Container> umapQuery(QueryResult qresult)
  {
    Container dest;
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
        // Creates a non-virtual Product
        ProductInfo pinfo(row, false);
        dest.emplace(pinfo.product_id, std::make_shared<Product>(Product(pinfo)));
        row.clear(); // Limpiar el mapa para la proxima fila
      }
    }
    return dest;
  }

  // Operators

  operator bool() const noexcept
  {
    return db;
  }

  Manager &operator=(Manager &&other)
  {
    if (this != &other)
    {
      db = other.db;
      should_free_db = other.should_free_db;
      cache = std::move(other.cache);
    }
    return *this;
  }
};

#endif