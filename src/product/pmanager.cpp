#include "product/pmanager.hpp"

// Tipos especificos para este archivo de implementacion

using Pm = ProductManager;

template <typename T>
using Sec = Pm::SecureReturn<T>;

Sec<Product<true>> Pm::secGetProduct(const std::uint64_t id) noexcept
{
  using BaseErrMsgs = Manager<Product<true>>::ErrMsgs;

  // Decrease cache relevance of products and remove irrelevant products
  vec<int> garbage_products;
  for (auto pair : cache)
  {
    if (pair.first != id)
    {
      if (!pair.second->decCacheRel())
      {
        garbage_products.push_back(pair.first);
      }
    }
  }
  for (int each_id : garbage_products)
  {
    cache.erase(each_id);
  }

  // Returns the product if it is in cache
  auto it = cache.find(id);
  if (it != cache.cend())
    return std::make_pair(std::optional<Product<true>>(*(it->second)), "");

  if (!db)
    return std::make_pair(std::nullopt, BaseErrMsgs::DB_IS_NULL);

  // Obtains the product from database
  try
  {
    db->executeQuery("SELECT * FROM products_info as p WHERE p.id = $", {std::to_string(id)});
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::nullopt, e.what());
  }

  auto cont = extractContainer(db->fetchQuery());
  it = cont.find(id);
  if (it == cont.end())
  {
    return std::make_pair(std::nullopt, ErrMsgs::PRODUCT_NOT_FOUND);
  }

  addCache(it->second); // And finally we push the product to cache

  return std::make_pair(std::optional<Product<true>>(*(it->second)), "");
}

Product<true> Pm::getProduct(const std::uint64_t id)
{
  using BaseErrMsgs = Manager<Product<true>>::ErrMsgs;

  // Decrease cache relevance of products and remove irrelevant products
  vec<int> garbage_products;
  for (auto pair : cache)
  {
    if (pair.first != id)
    {
      if (!pair.second->decCacheRel())
      {
        garbage_products.push_back(pair.first);
      }
    }
  }
  for (int each_id : garbage_products)
  {
    cache.erase(each_id);
  }

  // Returns the product if it is in cache
  auto it = cache.find(id);
  if (it != cache.cend())
  {
    return *(it->second);
  }

  // Returns nullptr in case the product is not in cache and datbase is not initialized
  if (!db)
    throw std::runtime_error(BaseErrMsgs::DB_IS_NULL);

  // Obtains the product from database
  try
  {
    db->executeQuery("SELECT * FROM products_info as p WHERE p.id = $", {std::to_string(id)});
  }
  catch (const std::exception &e)
  {
    throw std::runtime_error(e.what());
  }

  auto cont = extractContainer(db->fetchQuery());
  it = cont.find(id);
  if (it == cont.end())
  {
    throw std::runtime_error(ErrMsgs::PRODUCT_NOT_FOUND);
  }

  addCache(it->second); // And finally we push the product to cache

  return *(it->second);
}

Sec<Product<true>> Pm::secCreateProduct(const Product<false> &p, const std::tuple<bool, bool> handle_tran) const noexcept
{
  try
  {
    if (std::get<0>(handle_tran))
      db->executeUpdate("BEGIN TRANSACTION");

    db->executeUpdate(
        "INSERT INTO products (name, description, serial, owner vendor_id, price, count) VALUES ($, $, $, $, $, $, $)",
        {"\"" + p.name + "\"",
         "\"" + p.description + "\"",
         std::to_string(p.vendor_id),
         std::to_string(p.price),
         std::to_string(p.count)});

    if (std::get<1>(handle_tran))
      db->executeUpdate("COMMIT");

    /* Recuperar el producto con ID mas grande (ultima insercion porque la base de datos trabaja con autoincrement para acelerar el rendimiento) */
    db->executeQuery("SELECT * FROM products as p products_info WHERE p.id = (SELECT MAX(p.id) FROM p)");

    auto cont = extractContainer(db->fetchQuery());
    return std::make_pair(std::optional<Product<true>>(*(cont.cbegin()->second)), "");
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::nullopt, e.what());
  }
}

// Product id is ignored (it is virtual)
Product<true> Pm::createProduct(const Product<false> &p, const std::tuple<bool, bool> handle_tran) const
{
  if (std::get<0>(handle_tran))
    db->executeUpdate("BEGIN TRANSACTION");

  db->executeUpdate(
      "INSERT INTO products (name, description, serial, owner vendor_id, price, count) VALUES ($, $, $, $, $, $, $)",
      {"\"" + p.name + "\"",
       "\"" + p.description + "\"",
       std::to_string(p.vendor_id),
       std::to_string(p.price),
       std::to_string(p.count)});

  if (std::get<1>(handle_tran))
    db->executeQuery("COMMIT");

  db->executeQuery("SELECT * FROM products_info as p WHERE p.id = (SELECT MAX(p.id) FROM p)");

  auto cont = extractContainer(db->fetchQuery());
  return *(cont.cbegin()->second);
}

Sec<std::uint64_t> Pm::secAddProduct(const std::uint64_t id, const std::tuple<bool, bool> handle_tran) noexcept
{
  std::uint64_t count;
  bool must_update_cache;

  try
  {
    if (std::get<0>(handle_tran))
      db->executeUpdate("BEGIN TRANSACTION");

    /* Incrementar en uno la cantidad para el registro con ID especificado */
    db->executeUpdate(
        "UPDATE FROM products as p WHERE p.id = $ SET p.count = p.count + 1", {std::to_string(id)});

    if (std::get<1>(handle_tran))
      db->executeQuery("COMMIT");

    /* Extrallendo la nueva cantidad de prodcutos */
    db->executeQuery(
        "SELECT * FROM products_info as p WHERE p.id = $", {std::to_string(id)});

    auto cont = extractContainer(db->fetchQuery());
    auto it = cont.find(id);
    if (it == cont.cend())
      throw std::runtime_error(ErrMsgs::ADD_PRODUCT_FAILED);

    count = it->second->count;

    /* Actualizando la cache en caso de que sea necesario */
    auto result = secGetProduct(id);
    must_update_cache = bool(result.first);
    remCache(id);

    if (must_update_cache)
      addCache(it->second);

    return std::make_pair(std::optional<std::uint64_t>(count), "");
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::nullopt, e.what());
  }
}

std::uint64_t Pm::addProduct(const std::uint64_t id, const std::tuple<bool, bool> handle_tran)
{
  std::uint64_t count;
  bool must_update_cache;

  if (std::get<0>(handle_tran))
    db->executeUpdate("BEGIN TRANSACTION");

  /* Incrementar en uno la cantidad para el registro con ID especificado */
  db->executeUpdate(
      "UPDATE FROM products as p WHERE p.id = $ SET p.count = p.count + 1", {std::to_string(id)});

  if (std::get<1>(handle_tran))
    db->executeQuery("COMMIT");

  /* Extrallendo la nueva cantidad de prodcutos */
  db->executeQuery(
      "SELECT * FROM products_info as p WHERE p.id = $", {std::to_string(id)});

  auto cont = extractContainer(db->fetchQuery());
  auto it = cont.find(id);
  if (it == cont.cend())
    throw std::runtime_error(ErrMsgs::ADD_PRODUCT_FAILED);

  count = it->second->count;

  /* Actualizar la cache en caso de ser necesario */
  auto result = secGetProduct(id);
  must_update_cache = bool(result.first);
  remCache(id);
  if (must_update_cache)
    addCache(it->second);

  return count;
}

Sec<std::uint64_t> Pm::secRemoveProduct(std::uint64_t id, const std::tuple<bool, bool> handle_tran) noexcept
{
  try
  {
    if (std::get<0>(handle_tran))
      db->executeUpdate("BEGIN TRANSACTION");

    db->executeUpdate("DELETE FROM products as p WHERE p.product_id = $",
                      {std::to_string(id)});

    if (std::get<1>(handle_tran))
      db->executeUpdate("COMMIT");

    db->executeQuery("SELECT * FROM products_info as p WHERE p.id = $", {std::to_string(id)});

    auto cont = extractContainer(db->fetchQuery());
    if (cont.cbegin()->first == id)
      throw std::runtime_error(ErrMsgs::DELETE_PRODUCT_FAILED);

    remCache(id); // Es eliminado de la cache
    return std::make_pair(std::optional<int>(id), "");
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::nullopt, e.what());
  }
}

std::uint64_t Pm::removeProduct(std::uint64_t id, const std::tuple<bool, bool> handle_tran)
{
  if (std::get<0>(handle_tran))
    db->executeUpdate("BEGIN TRANSACTION");

  db->executeUpdate("DELETE FROM products as p WHERE p.product_id = $",
                    {std::to_string(id)});

  if (std::get<1>(handle_tran))
    db->executeUpdate("COMMIT");

  db->executeQuery("SELECT * FROM products_info as p WHERE p.id = $", {std::to_string(id)});

  auto cont = extractContainer(db->fetchQuery());
  if (cont.cbegin()->first == id)
    throw std::runtime_error(ErrMsgs::DELETE_PRODUCT_FAILED);

  remCache(id); // Es eliminado de la cache
  return id;
}