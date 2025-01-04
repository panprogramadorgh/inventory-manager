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

  // Methods virtuales puros

  virtual std::uint64_t addCache(std::shared_ptr<T>) noexcept = 0;

  virtual std::uint64_t remCache(const std::uint64_t) noexcept = 0;

  // Metodos estaticos en linea

  static Container extractContainer(QueryResult qresult)
  {
    Container dest;
    typename T::RecordUmap record;

    auto cols = qresult.first;
    auto vals = qresult.second;

    for (auto it = vals.cbegin(); it != vals.cend(); it += cols.size())
    {
      for (auto it2 = it; it2 != it + cols.size(); it2++)
      {
        record[std::distance(it, it2)] = *it2;
        if (it2 == it + cols.size() - 1)
          dest.emplate(
              std::stoull(record.at(0)),
              std::make_shared<T>(T(record, false)))
      }
    }
    return dest;
  }

  // Metodos no estaticos en linea

  void printContainer(const Container cont) noexcept
  {
    std::uint16_t i; // Permite deducir si hace falta colocar coma

    i = 0;
    for (const auto &pair : T::field_to_string)
    {
      if (i++)
        std::cout << ',';
      std::cout << cont->second;
    }
    i = 0;
    for (const auto &pair : T::field_to_string)
    {
      if (i++)
        std::cout << ',';
      std::cout << cont.at(pair.first);
    }
  }

  void printContainer(const QueryResult qresult) noexcept
  {
    auto cols = qresult.first;
    auto vals = qresult.second;

    for (auto it = cols.cbegin(); it != cols.cend(); it++)
      std::cout << *it << (it == cols.cend() - 1 ? "\n" : ",");
    for (auto it = vals.cbegin(); it != vals.cend(); it++)
      std::cout << *it << (std::distance(vals.cbegin(), it) % cols.size() == cols.size() - 1 ? "\n" : ",");
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