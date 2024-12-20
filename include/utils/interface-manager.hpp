#ifndef INTERFACE_MANAGER_HPP
#define INTERFACE_MANAGER_HPP

#include "database/interface.hpp"
#include "forwarder.hpp"
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

  template <typename U = T>
  static QueryUmap<U> umapQuery(QueryResult qresult);

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