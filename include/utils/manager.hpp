#ifndef INTERFACE_MANAGER_HPP
#define INTERFACE_MANAGER_HPP

#include "forwarder.hpp"
#include "database/dbinterface.hpp"
#include "utils/manager-item.hpp"
#include "utils/database-utils.hpp"
#include <memory>
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
  // Underlayed database connection
  std::unique_ptr<Database> db;

  // Caches last queries to sql_view
  Container cache;

  // Used for sql updates
  std::string sql_table;
  // Used for sql queries
  std::string sql_view;

  struct ErrMsgs
  {
    static constexpr char ITEM_NOT_FOUND[] = "Could not find specified item/s";
    static constexpr char CREATE_ITEM_FAILED[] = "Could not create new item";
    static constexpr char DELETE_ITEM_FAILED[] = "Could not delete specified item/s";
  };

public:
  /// @brief Standard constructor
  /// @param dbfile File path to sqlite database file
  /// @param updater Sql table where all ManagerItems are stored as records
  /// @param viewer Sql view which contains all resolved foreign key references (data queried from this view is used to build ManagerItem instances)
  Manager(const std::string dbfile, const std::string updater, const std::string viewer)
      : db(nullptr), sql_table(updater), sql_view(viewer)
  {
    *db = Database(dbfile);
    db->connect();
  }

  /// @brief Constructor from open database instance
  /// @param db Manager database initializer
  /// @param updater
  /// @param viewer
  /// @todo Must receive a connected database (Database::connect()), so find a way to check if database is connected
  Manager(Database &db, const std::string updater, const std::string viewer)
      : db(db), sql_table(updater), sql_view(viewer)
  {
    if (!*db)
      throw std::invalid_argument(DatabaseError::ErrMsgs::DB_IS_NOT_OPEN);
  }

  Manager(const Manager<T> &other) = delete;

  Manager(Manager<T> &&other)
      : db(std::move(other.db)),
        cache(std::move(other.cache)),
        sql_table(std::move(other.sql_table)),
        sql_view(std::move(other.sql_view))
  {
  }

  // Container related static methods

  static Container extractContainer(QueryResult qresult)
  {
    Container dest;
    typename T::RecordUmap record;

    auto cols = qresult.first;
    auto vals = qresult.second;

    if (vals.size() % cols.size())
      throw std::invalid_argument("Manager<T>::exctractContainer Invalid query result");

    for (auto it = vals.cbegin(); it != vals.cend(); it += cols.size())
    {
      record.clear();
      for (auto it2 = cols.cbegin(); it2 != cols.cend(); it2++)
      {
        record[T::string_to_field.at(*it2)] = *(it + std::distance(cols.cbegin(), it2));

        if (record.size() == cols.size())
        {
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

  // Database manipulation non static methods (since they are generic, they must defined inline)

  SecureReturn<T> secGetItem(const std::uint64_t id) noexcept
  {
    // Decrease cache relevance of items and remove irrelevant items
    vec<int> garbage_items;
    for (auto pair : cache)
    {
      if (pair.first != id)
      {
        if (!pair.second->decCacheRel())
        {
          garbage_items.push_back(pair.first);
        }
      }
    }
    for (int each_id : garbage_items)
    {
      cache.erase(each_id);
    }

    // Returns the item if it is in cache
    auto it = cache.find(id);
    if (it != cache.cend())
      return std::make_pair(std::optional<T>(*(it->second)), "");

    // Obtains the item from database
    try
    {
      db->executeQuery(DatabaseUtils::createQuery(sql_table, sql_viewer, "SELECT * FROM # as p WHERE p.id = $", {std::to_string(id)}));

      auto cont = extractContainer(db->fetchQuery());
      it = cont.find(id);
      if (it == cont.end())
        ErrMsgs::ITEM_NOT_FOUND;
    }
    catch (const std::exception &e)
    {
      return std::make_pair(std::nullopt, e.what());
    }

    addCache(it->second); // And finally we push the item to cache

    return std::make_pair(std::optional<T>(*(it->second)), "");
  }

  virtual T getItem(const std::uint64_t id)
  {
    // Decrease cache relevance of items and remove irrelevant items
    vec<int> garbage_items;
    for (auto pair : cache)
    {
      if (pair.first != id)
      {
        if (!pair.second->decCacheRel())
        {
          garbage_items.push_back(pair.first);
        }
      }
    }
    for (int each_id : garbage_items)
    {
      cache.erase(each_id);
    }

    // Returns the item if it is in cache
    auto it = cache.find(id);
    if (it != cache.cend())
      return std::make_pair(std::optional<T>(*(it->second)), "");

    // Obtains the item from database
    db->executeQuery(DatabaseUtils::createQuery(sql_table, sql_viewer, "SELECT * FROM # as p WHERE p.id = $", {std::to_string(id)}));

    auto cont = extractContainer(db->fetchQuery());
    it = cont.find(id);
    if (it == cont.end())
      throw std::runtime_error(ErrMsgs::ITEM_NOT_FOUND);

    addCache(it->second); // And finally we push the item to cache

    return *(it->second);
  }

  virtual SecureReturn<T> secCreateItem(const T &p, const std::pair<bool> begin_commit = std::make_pair(true, true)) const noexcept
  {
    try
    {
      if (begin_commit.first)
        db->executeUpdate("BEGIN TRANSACTION");

      std::string query =
          DatabaseUtils::createQuery(
              sql_table,
              sql_view,
              "INSERT INTO % (name, description, serial, owner vendor_id, price, count) VALUES ($, $, $, $, $, $, $)",
              {"\"" + p.name + "\"",
               "\"" + p.description + "\"",
               std::to_string(p.vendor_id),
               std::to_string(p.price),
               std::to_string(p.count)});

      db->executeUpdate(query);

      if (begin_commit.second)
        db->executeUpdate("COMMIT");

      // Fetching latest item inserted (with higher i)
      query =
          DatabaseUtils::createQuery(
              sql_table,
              sql_view,
              "SELECT * FROM % as p # WHERE p.id = (SELECT MAX(p.id) FROM p)");

      db->executeQuery(query);

      auto cont = extractContainer(db->fetchQuery());
      if (cont.size() < 1)
        throw std::runtime_error(ErrMsgs::CREATE_ITEM_FAILED);

      return std::make_pair(std::optional<T>(*(cont.cbegin()->second)), "");
    }
    catch (const std::exception &e)
    {
      return std::make_pair(std::nullopt, e.what());
    }
  }

  virtual T createItem(const T &p, const std::pair<bool> begin_commit = std::make_pair(true, true)) const
  {
    if (begin_commit.first)
      db->executeUpdate("BEGIN TRANSACTION");

    std::string query =
        DatabaseUtils::createQuery(
            sql_table,
            sql_view,
            "INSERT INTO % (name, description, serial, owner vendor_id, price, count) VALUES ($, $, $, $, $, $, $)",
            {"\"" + p.name + "\"",
             "\"" + p.description + "\"",
             std::to_string(p.vendor_id),
             std::to_string(p.price),
             std::to_string(p.count)});

    db->executeUpdate(query);

    if (begin_commit.second)
      db->executeUpdate("COMMIT");

    // Fetching latest product inserted (with higher i)
    query =
        DatabaseUtils::createQuery(
            sql_table,
            sql_view,
            "SELECT * FROM % as p # WHERE p.id = (SELECT MAX(p.id) FROM p)");

    db->executeQuery(query);

    auto cont = extractContainer(db->fetchQuery());
    if (cont.size() < 1)
      throw std::runtime_error(ErrMsgs::CREATE_ITEM_FAILED);

    return *(cont.cbegin()->second));
  }

  /*
   TODO:
    Metodos de modificacion de items genericos
  */
  // SecureReturn<std::uint64_t> secAddProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran) noexcept;
  // std::uint64_t addProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran);

  virtual SecureReturn<std::uint64_t> secRemoveItem(std::uint64_t id, const std::pair<bool> begin_commit = {}) noexcept
  {
    try
    {
      if (begin_commit.first)
        db->executeUpdate("BEGIN TRANSACTION");

      std::string query =
          DatabaseUtils::createQuery(
              sql_table,
              sql_view,
              "DELETE FROM % as p WHERE p.product_id = $",
              {std::to_string(id)});

      db->executeUpdate(query);

      if (begin_commit.second)
        db->executeUpdate("COMMIT");

      // Verifies the item was deleted
      query =
          DatabaseUtils::createQuery(
              sql_table,
              sql_view, "SELECT * FROM # as p WHERE p.id = $", {std::to_string(id)});

      db->executeQuery(query);

      auto cont = extractContainer(db->fetchQuery());
      if (cont.cbegin()->first == id)
        throw std::runtime_error(ErrMsgs::DELETE_ITEM_FAILED);

      remCache(id); // Es eliminado de la cache
      return std::make_pair(std::optional<int>(id), "");
    }
    catch (const std::exception &e)
    {
      return std::make_pair(std::nullopt, e.what());
    }
  }

  virtual std::uint64_t removeItem(std::uint64_t id, const std::pair<bool> begin_commit = {}) noexcept
  {
    if (begin_commit.first)
      db->executeUpdate("BEGIN TRANSACTION");

    std::string query =
        DatabaseUtils::createQuery(
            sql_table,
            sql_view,
            "DELETE FROM % as p WHERE p.product_id = $",
            {std::to_string(id)});

    db->executeUpdate(query);

    if (begin_commit.second)
      db->executeUpdate("COMMIT");

    // Verifies the item was deleted
    query =
        DatabaseUtils::createQuery(
            sql_table,
            sql_view, "SELECT * FROM # as p WHERE p.id = $", {std::to_string(id)});

    db->executeQuery(query);

    auto cont = extractContainer(db->fetchQuery());
    if (cont.cbegin()->first == id)
      throw std::runtime_error(ErrMsgs::DELETE_ITEM_FAILED);

    remCache(id); // Es eliminado de la cache
    return id;
  }

  // Cache methods

  std::uint64_t addCache(std::shared_ptr<T> item) noexcept
  {
    if (item->isVirtual())
      return 0;

    auto it = cache.find(item->id);
    if (it != cache.cend()) // In case the item does not exist in cache, it is been added.
      return 0;
    cache.emplace(item->id, p);
    return cache.size();
  }

  std::uint64_t remCache(std::uint64_t id) noexcept
  {
    cache.erase(id);
    return cache.size();
  }

  // Operators

  bool operator bool() const noexcept
  {
    return bool(*db);
  }

  Manager &operator=(const Manager &other) = delete;

  Manager &operator=(Manager &&other)
  {
    if (this != &other)
    {
      db = std::move(other.db);
      cache = std::move(other.cache);
      sql_table = std::move(other.sql_table);
      sql_view = std::move(other.sql_view);
    }
    return *this;
  }

  virtual ~Manager() = default;
};

#endif