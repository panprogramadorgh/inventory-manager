#include "product/manager.hpp"

// Public methods

ProductManager::SecureReturn<ProductInfo> ProductManager::secGetProduct(const int id) noexcept
{
  // Decrease cache relevance of products and remove irrelevant products
  std::vector<int> garbage_products;
  for (auto pair : cached_products)
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
    cached_products.erase(each_id);
  }

  // Returns the product if it is in cache
  auto it = cached_products.find(id);
  if (it != cached_products.cend())
    return std::make_pair(std::make_shared<ProductInfo>(it->second->info()), std::string(""));

  if (!db)
    return std::make_pair(std::make_shared<ProductInfo>(nullptr), std::string(ErrorMessages::DB_IS_NULL));

  // Obtains the product from database
  try
  {
    db->executeQuery("SELECT * FROM products_info WHERE product_id = $", {std::to_string(id)});
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::make_shared<ProductInfo>(nullptr), std::string(e.what()));
  }

  auto products = Database::umapQuery<Product>(db->fetchQuery());
  it = products.find(id);
  if (it == products.end())
  {
    return std::make_pair(std::make_shared<ProductInfo>(nullptr), std::string(ErrorMessages::PRODUCT_NOT_FOUND));
  }

  addProductToCache(it->second); // And finally we push the product to cache

  return std::make_pair(std::make_shared<ProductInfo>(it->second->info()), std::string(""));
}

ProductInfo ProductManager::getProduct(const int id)
{
  // Decrease cache relevance of products and remove irrelevant products
  std::vector<int> garbage_products;
  for (auto pair : cached_products)
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
    cached_products.erase(each_id);
  }

  // Returns the product if it is in cache
  auto it = cached_products.find(id);
  if (it != cached_products.cend())
  {
    return it->second->info();
  }

  // Returns nullptr in case the product is not in cache and datbase is not initialized
  if (!db)
    throw std::runtime_error(ErrorMessages::DB_IS_NULL);

  // Obtains the product from database
  try
  {
    db->executeQuery("SELECT * FROM products_info WHERE product_id = $", {std::to_string(id)});
  }
  catch (const std::exception &e)
  {
    throw std::runtime_error(e.what());
  }

  auto products = Database::umapQuery<Product>(db->fetchQuery());
  it = products.find(id);
  if (it == products.end())
  {
    throw std::runtime_error(ErrorMessages::PRODUCT_NOT_FOUND);
  }

  addProductToCache(it->second); // And finally we push the product to cache

  return it->second->info();
}

// Product id is ignored (it is virtual)
ProductManager::SecureReturn<ProductInfo> ProductManager::secAddProduct(const ProductInfo &p, const int vendor_id, const bool commit_update) noexcept
{
  try
  {
    db->executeUpdate(
        "INSERT INTO products (product_name, product_description, vendor_id, product_count, product_price) VALUES ($, $, $, $, $)",
        {"\"" + p.product_name + "\"",
         "\"" + p.product_description + "\"",
         std::to_string(vendor_id),
         std::to_string(p.product_price),
         std::to_string(p.product_count)});
    if (commit_update)
      db->executeQuery("COMMIT");
    db->executeQuery("SELECT * FROM products_info WHERE product_id = max(SELECT product_id FROM products_info)");

    auto result = Database::umapQuery<ProductInfo>(db->fetchQuery());
    return std::make_pair(result.cbegin()->second, std::string(""));
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::make_shared<ProductInfo>(nullptr), std::string(e.what()));
  }
}

// Product id is ignored (it is virtual)
ProductInfo ProductManager::addProduct(const ProductInfo &p, const int vendor_id, const bool commit_update)
{
  db->executeUpdate(
      "INSERT INTO products (product_name, product_description, vendor_id, product_count, product_price) VALUES ($, $, $, $, $)",
      {"\"" + p.product_name + "\"",
       "\"" + p.product_description + "\"",
       std::to_string(vendor_id),
       std::to_string(p.product_price),
       std::to_string(p.product_count)});
  if (commit_update)
    db->executeQuery("COMMIT");
  db->executeQuery("SELECT * FROM products_info WHERE product_id = max(SELECT product_id FROM products_info)");
  auto result = Database::umapQuery<ProductInfo>(db->fetchQuery());
  return *(result.cbegin()->second);
}

ProductManager::SecureReturn<int> ProductManager::secRemoveProduct(const int id, const bool commit_update) noexcept
{
  try
  {
    db->executeUpdate("DELETE FROM products as p WHERE p.product_id = $",
                      {std::to_string(id)});
    db->executeQuery("SELECT * FROM products as p WHERE p.product_id = $", {std::to_string(id)});
    auto result = Database::umapQuery<ProductInfo>(db->fetchQuery());
    for (const auto &p : result)
    {
      if (p.first == id)
        throw std::runtime_error("Coud not delete product");
    }
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::shared_ptr<int>(nullptr), std::string(e.what()));
  }

  removeProductFromCache(id); // Es eliminado de la cache

  if (commit_update)
    db->executeUpdate("COMMIT");

  return std::make_pair(std::make_shared<int>(id), std::string(""));
}

int ProductManager::removeProduct(const int id, const bool commit_update)
{
  db->executeUpdate("DELETE FROM products as p WHERE p.product_id = $",
                    {std::to_string(id)});
  db->executeQuery("SELECT * FROM products_info as p WHERE p.product_id = $", {std::to_string(id)});
  auto result = Database::umapQuery<ProductInfo>(db->fetchQuery());
  for (const auto &p : result)
  {
    if (p.first == id)
      throw std::runtime_error("Coud not delete product");
  }
  removeProductFromCache(id); // Es eliminado de la cache
  if (commit_update)
    db->executeUpdate("COMMIT");
}

// Private methods

std::size_t ProductManager::addProductToCache(std::shared_ptr<Product> p) noexcept
{
  // Do not add product if it is virtual
  if (p->identifier() == -1)
    return 0;

  auto it = cached_products.find(p->identifier());
  if (it != cached_products.cend()) // In case the product does not exist in cache, it is been added.
    return 0;

  cached_products.emplace(p->identifier(), p);
  return cached_products.size();
}

std::size_t ProductManager::removeProductFromCache(int id) noexcept
{
  cached_products.erase(id);
  return cached_products.size();
}