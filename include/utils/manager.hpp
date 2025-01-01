#ifndef INTERFACE_MANAGER_HPP
#define INTERFACE_MANAGER_HPP

#include "forwarder.hpp"
#include "database/interface.hpp"
#include "utils/manager-item.hpp"
#include <optional>

template <IsManagerItem T>
class Manager
{
public:
  // Types
  using Container = umap<const std::uint64_t, std::shared_ptr<T>>;

  template <typename U = T>
  using SecureReturn = std::pair<std::optional<U>, std::string>;

protected:
  Database *db;
  bool should_free_db;

  Container cache;

  /* Mensajes de error genericos */
  struct ErrMsgs
  {
    static constexpr char DB_IS_NULL[] = "Database is null";
  };

public:
  Manager(const std::string dbfile)
      : db(nullptr), should_free_db(true)
  {
    db = new Database(dbfile);
    db->connect();
  }

  // Must receive a connected database (Database::connect())
  Manager(const Database *db, const bool should_free_db = false)
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

  // Methods "interface"

  virtual std::uint64_t addCache(std::shared_ptr<T>) noexcept = 0;

  virtual std::uint64_t remCache(const std::uint64_t) noexcept = 0;

  // Metodos estaticos en linea

  static Container extractContainer(QueryResult qresult)
  {
    Container dest;
    T::RecordUmap record;
    T::RecordField field;

    auto cols = qresult.first;
    auto vals = qresult.second;

    for (auto it = vals.begin();
         it != vals.cend() &&
         std::distance(vals.begin(), it) % cols.size() < cols.size();
         it++)
    {
      field = T::string_to_field.at(cols[std::distance(vals.begin(), it) % cols.size()]);
      record[field] = *it;

      // En caso de que sea la ulima columna por registro
      if (std::distance(vals.begin(), it) % cols.size() == cols.size() - 1)
      {
        // Creates a non-virtual manager item
        dest.emplace(pinfo.product_id, std::make_shared<T>(T(record, false)));
        record.clear(); // Limpiar el mapa para la proxima fila
      }
    }
    return dest;
  }

  // Operators

  operator bool() const noexcept
  {
    return db;
  }

  Manager &operator=(const Manager &other) = delete;

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