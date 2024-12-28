#include "product/pmanager.hpp"

// Public methods

ProductManager::SecureReturn<Product> ProductManager::secGetProduct(std::uint64_t id) noexcept
{
  // Decrease cache relevance of products and remove irrelevant products
  vec<int> garbage_products;
  for (auto pair : cache)
  {
    if (pair.first != id)
    {
      if (!pair.second->decreaseCacheRelevance())
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
    return std::make_pair(std::optional<Product>(it->second->info()), "");

  if (!db)
    return std::make_pair(std::nullopt, ErrMsgs::Manager::DB_IS_NULL);

  // Obtains the product from database
  try
  {
    db->executeQuery("SELECT * FROM products_info as p WHERE p.id = $", {std::to_string(id)});
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::nullopt, e.what());
  }

  auto products = extractContainer(db->fetchQuery());
  it = products.find(id);
  if (it == products.end())
  {
    return std::make_pair(std::nullopt, ErrMsgs::PRODUCT_NOT_FOUND);
  }

  addCache(it->second); // And finally we push the product to cache

  return std::make_pair(std::optional<Product>(it->second->info()), "");
}

Product ProductManager::getProduct(std::uint64_t id)
{
  // Decrease cache relevance of products and remove irrelevant products
  vec<int> garbage_products;
  for (auto pair : cache)
  {
    if (pair.first != id)
    {
      if (!pair.second->decreaseCacheRelevance())
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
    return it->second->info();
  }

  // Returns nullptr in case the product is not in cache and datbase is not initialized
  if (!db)
    throw std::runtime_error(Manager::ErrMsgs::DB_IS_NULL);

  // Obtains the product from database
  try
  {
    db->executeQuery("SELECT * FROM products_info as p WHERE p.id = $", {std::to_string(id)});
  }
  catch (const std::exception &e)
  {
    throw std::runtime_error(e.what());
  }

  auto products = extractContainer(db->fetchQuery());
  it = products.find(id);
  if (it == products.end())
  {
    throw std::runtime_error(ErrMsgs::PRODUCT_NOT_FOUND);
  }

  addCache(it->second); // And finally we push the product to cache

  return it->second->info();
}

// Product id is ignored (it is virtual)
ProductManager::SecureReturn<Product> ProductManager::secCreateProduct(const Product &p, std::uint64_t vendor_id, const std::tuple<bool, bool> handle_tran) noexcept
{
  try
  {
    if (std::get<0>(handle_tran))
      db->executeUpdate("BEGIN TRANSACTION");
    db->executeUpdate(
        "INSERT INTO products (name, description, serial, owner vendor_id, price, count) VALUES ($, $, $, $, $, $, $)",
        {"\"" + p.name + "\"",
         "\"" + p.description + "\"",
         std::to_string(vendor_id),
         std::to_string(p.product_price),
         std::to_string(p.product_count)});
    if (std::get<1>(handle_tran))
      db->executeUpdate("COMMIT");
    db->executeQuery("SELECT * FROM products_info WHERE product_id = (SELECT MAX(product_id) FROM products_info)");

    auto qumap = extractContainer(db->fetchQuery());
    return std::make_pair(std::optional<Product>(*(qumap.cbegin()->second)), std::string());
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::nullopt, e.what());
  }
}

// Product id is ignored (it is virtual)
Product ProductManager::createProduct(const Product &p, std::uint64_t vendor_id, const std::tuple<bool, bool> handle_tran)
{
  if (std::get<0>(handle_tran))
    db->executeUpdate("BEGIN TRANSACTION");
  db->executeUpdate(
      "INSERT INTO products (product_name, product_description, vendor_id, product_count, product_price) VALUES ($, $, $, $, $)",
      {"\"" + p.product_name + "\"",
       "\"" + p.product_description + "\"",
       std::to_string(vendor_id),
       std::to_string(p.product_price),
       std::to_string(p.product_count)});
  if (std::get<1>(handle_tran))
    db->executeQuery("COMMIT");
  db->executeQuery("SELECT * FROM products_info WHERE product_id = (SELECT MAX(product_id) FROM products_info)");
  auto qumap = extractContainer(db->fetchQuery());
  return *(qumap.cbegin()->second);
}

ProductManager::SecureReturn<std::uint64_t> ProductMAnager::secAddProduct(const std::uint64_t product_id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true)) noexcept
{
  if (std::get<0>(handle_tran))
    db->executeUpdate("BEGIN TRANSACTION");

  db->executeUpdate(
      "UPDATE FROM products as p WHERE p.product_id = $ SET p.product_count = p.product_count + 1", {std::to_string(product_id)});
  db->executeQuery(
      "SELECT * FROM products_info as p WHERE p.product_id = $", {std::to_string(product_id)});

  // FIXME: Terminar

  if (std::get<1>(handle_tran))
    db->executeQuery("COMMIT");
}

ProductManager::SecureReturn<int> ProductManager::secRemoveProduct(std::uint64_t id, const std::tuple<bool, bool> handle_tran) noexcept
{
  try
  {
    if (std::get<0>(handle_tran))
      db->executeUpdate("BEGIN TRANSACTION");
    db->executeUpdate("DELETE FROM products as p WHERE p.product_id = $",
                      {std::to_string(id)});
    if (std::get<1>(handle_tran))
      db->executeUpdate("COMMIT");
    db->executeQuery("SELECT * FROM products_info as p WHERE p.product_id = $", {std::to_string(id)});
    auto qumap = extractContainer(db->fetchQuery());
    if (qumap.cbegin()->first == id)
      throw std::runtime_error(ProductManager::ErrMsgs::DELETE_PRODUCT_FAILED);
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::nullopt, e.what());
  }
  remCache(id); // Es eliminado de la cache
  return std::make_pair(std::optional<int>(id), "");
}

int ProductManager::removeProduct(std::uint64_t id, const std::tuple<bool, bool> handle_tran)
{
  if (std::get<0>(handle_tran))
    db->executeUpdate("BEGIN TRANSACTION");
  db->executeUpdate("DELETE FROM products as p WHERE p.product_id = $",
                    {std::to_string(id)});
  if (std::get<1>(handle_tran))
    db->executeUpdate("COMMIT");
  db->executeQuery("SELECT * FROM products_info as p WHERE p.product_id = $", {std::to_string(id)});
  auto qumap = extractContainer(db->fetchQuery());
  if (qumap.cbegin()->first == id)
    throw std::runtime_error(ProductManager::ErrMsgs::DELETE_PRODUCT_FAILED);
  remCache(id); // Es eliminado de la cache
  return id;
}