#include "product/manager.hpp"

// Public methods

ProductInfo *ProductManager::getProduct(const int id) noexcept
{
  ProductInfo *p = (ProductInfo *)nullptr;

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
    p = new ProductInfo(it->second->info());
    return p;
  }

  // Returns nullptr in case the product is not in cache and datbase is not initialized
  if (!db)
    return p;

  // Obtains the product from database
  try
  {
    db->executeQuery("SELECT * FROM products_info WHERE product_id = $", {std::to_string(id)});
  }
  catch (const std::exception &e)
  {
    return p;
  }

  auto products = Database::umapQuery(QueryUmap(), db->fetchQuery());
  it = products.find(id);
  if (it == products.end())
  {
    return p;
  }

  addProductToCache(it->second); // And finally we push the product to cache

  p = new ProductInfo(it->second->info());
  return p;
}

// Product id is ignored (it is virtual)
void ProductManager::addProduct(const ProductInfo &p, const int vendor_id, const bool commit_update) noexcept(false)
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
}

void ProductManager::removeProduct(const int id, const bool commit_update) noexcept
{
  db->executeUpdate("DELETE FROM products as p WHERE p.product_id = $",
                    {std::to_string(id)});
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