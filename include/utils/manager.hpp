#ifndef INTERFACE_MANAGER_HPP
#define INTERFACE_MANAGER_HPP

#include "forwarder.hpp"
#include "database/interface.hpp"
#include "utils/manager-item.hpp"
#include <optional>

template <ManagerItemBased T>
class Manager
{
public:
  // Types
  using Container = umap<std::uint64_t, std::shared_ptr<T>>;

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
  Manager(Database *db, const bool should_free_db = false)
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

  // Metodos estaticos

  static Container extractContainer(QueryResult qresult)
  {
    Container dest;
    typename T::RecordUmap record;

    auto cols = qresult.first;
    auto vals = qresult.second;

    for (auto it = vals.cbegin(); it != vals.cend(); it += cols.size())
    {
      record.clear();
      for (auto it2 = cols.cbegin(); it2 != cols.cend(); it2++)
      {
        record[T::string_to_field.at(*it2)] = *(it + std::distance(cols.cbegin(), it2));

        if (record.size() == cols.size())
        {
          // DEBUG:
          // std::cout << record.at(0) << " - " << record.at(0).size() << std::endl;

          dest.emplace(
              std::stoull(record.at(0)),
              std::make_shared<T>(T(record, false)));
        }
      }
    }
    return dest;
  }

  static void printContainer(const Container cont) noexcept
  {
    std::uint16_t i; // Permite deducir si hace falta colocar coma

    i = 0;
    for (const auto &pair : T::field_to_string)
    {
      if (i++)
        std::cout << ',';
      std::cout << pair.second;
    }
    i = 0;
    for (const auto &pair : T::field_to_string)
    {
      if (i++)
        std::cout << ',';
      std::cout << cont.at(pair.first);
    }
  }

  std::uint64_t addCache(std::shared_ptr<T> p) noexcept
  {
    // Do not add product if it is virtual
    if (p->isVirtual())
      return 0;

    auto it = cache.find(p->id);
    if (it != cache.cend()) // In case the product does not exist in cache, it is been added.
      return 0;
    cache.emplace(p->id, p);
    return cache.size();
  }

  std::uint64_t remCache(std::uint64_t id) noexcept
  {
    cache.erase(id);
    return cache.size();
  }

  // Operators

  operator bool() const noexcept
  {
    return db;
  }

  Manager &operator=(const Manager &other) = delete;

  virtual Manager &operator=(Manager &&other)
  {
    if (this != &other)
    {
      db = other.db;
      should_free_db = other.should_free_db;
      cache = std::move(other.cache);
    }
    return *this;
  }

  virtual ~Manager()
  {
    if (should_free_db)
      delete db;
    db = nullptr;
  }
};

#endif