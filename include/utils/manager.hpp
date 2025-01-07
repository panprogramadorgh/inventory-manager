#ifndef INTERFACE_MANAGER_HPP
#define INTERFACE_MANAGER_HPP

#include "forwarder.hpp"
#include "database/dbinterface.hpp"
#include "utils/manager-item.hpp"
#include <optional>

/*
[Warning]
  It's important to note that the Manager class is a template class that receives a ManagerItemBased class as a parameter.

  That ManagerItem must define its own database record mapping (string_to_field, field_to_string) which should begin with id field as first element -- otherwise, extractContainer would't be able to infer the id field to build the container.
*/

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

  /*
  [FIXED]
    FIXME:

    The cause of the error is that SmartProductBase (that way SmartProduct consequently too) doesn't have vendor_name field, but the query is trying to fetching a view that contains that field.

    This means that we will need to modify SmartProduct class in order to admin both circusntances (with vendor_name or with vendor_id field). I suppose that derivated classes like SmartProduct will inherit from the vendor-name-version of SmartProductBase.

  */
  static Container extractContainer(QueryResult qresult)
  {
    Container dest;
    typename T::RecordUmap record;

    auto cols = qresult.first;
    auto vals = qresult.second;

    // DEBUG:
    // std::cout << "Cols:" << std::endl;
    // for (const auto &col : cols)
    // {
    //   std::cout << col << std::endl;
    // }
    // std::cout << "Vals:" << std::endl;
    // for (const auto &val : vals)
    //   std::cout << val << std::endl;

    // for (const auto &strfield : T::string_to_field)
    // std::cout << strfield.first << std::endl;

    if (vals.size() % cols.size())
      throw std::invalid_argument("Manager<T>::exctractContainer Invalid query result");

    for (auto it = vals.cbegin(); it != vals.cend(); it += cols.size())
    {
      record.clear();
      for (auto it2 = cols.cbegin(); it2 != cols.cend(); it2++)
      {
        record[T::string_to_field.at(*it2)] = *(it + std::distance(cols.cbegin(), it2));

        // DEBUG: Prints whenever a record field is set
        // std::cout << record.cbegin()->first << " : " << record.cbegin()->second << std::endl;

        if (record.size() == cols.size())
        {
          /*
            FIXME:
            Constructor for T is not working since it is not initializing the fields of T from record
          */
          dest.emplace(
              std::stoull(record.at(0)),
              std::make_shared<T>(T(record, false)));
        }
      }
    }
    return dest;
  }

  static void
  printContainer(const Container cont) noexcept
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