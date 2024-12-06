#include "product/manager.hpp"
#include "database/interface.hpp"
#include <list>

// Constructors

ProductManager::ProductManager(Database *db)
    : db(db)
{
}

ProductManager::ProductManager(const ProductManager &other)
{
  db = other.db;
  cached_products = other.cached_products;
}

ProductManager &ProductManager::init(std::string init_file, std::function<void(Database &, std::string)> db_initializer)
{
  db->connect();
  db_initializer(*db, init_file);
  return *this;
}

// Public methods

const Product *ProductManager::getProduct(int id) noexcept
{
  // It allocates a block of memory in the heap, after obtaining the data, these are written in the Product object located in this block of memory (assignment operator = copies the data) and finally returns a pointer to it.
  Product *p = new Product();

  // Decrease cache relevance of products and remove irrelevant products
  std::vector<int> garbage_products;
  for (auto pair : cached_products)
  {
    if (pair.first != id)
    {
      if (!pair.second.get()->decreaseCacheRelevance())
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
    *p = *it->second.get(); // Copies the product inside the heap
    return p;
  }

  // Returns nullptr in case the product is not in cache and datbase is not initialized
  if (!db)
  {
    delete p;
    return nullptr;
  }

  // Obtains the product from database
  try
  {
    db->executeQuery("SELECT * FROM products_info as p WHERE p.product_id = $", {std::to_string(id)});
  }
  catch (const std::exception)
  {
    delete p;
    return nullptr;
  }

  std::cout << "Sql query was successfully run !" << std::endl;

  auto products = Database::parseQueryToUmap(std::unordered_map<int, std::shared_ptr<Product>>(), db->fetchQuery());
  it = products.find(id);
  if (it == products.end())
  {
    delete p;
    return nullptr;
  }

  addProductToCache(it->second); // And finally we push the product to cache

  *p = *it->second;
  return p;
}

// Product id is ignored (it is virtual)
void ProductManager::addProduct(const ProductInfo &p, const int vendor_id, const bool commit_update) noexcept(false)
{
  if (commit_update)
    db->executeUpdate("BEGIN TRANSACTION");

  db->executeUpdate(
      "INSERT INTO products (product_name, product_description, vendor_id, product_count, product_price) VALUES ($, $, $, $, $, $)",
      {"\"" + p.product_name + "\"",
       "\"" + p.product_description + "\"",
       std::to_string(vendor_id),
       std::to_string(p.product_price),
       std::to_string(p.product_count)});

  if (commit_update)
    db->executeQuery("COMMIT");
}

void ProductManager::removeProduct(const int id, const bool commit_update) noexcept(false)
{
  if (commit_update)
    db->executeUpdate("BEGIN TRANSACTION");

  db->executeUpdate("DELETE FROM products as p WHERE p.product_id = $",
                    {std::to_string(id)});
  removeProductFromCache(id); // Es eliminado de la cache

  if (commit_update)
    db->executeUpdate("COMMIT");
}

/*
void ProductManager::foo() noexcept(false)
{
  std::cout << "Cache: " << std::endl;
  for (auto pair : cached_products)
  {
    std::cout << pair.second->str() << std::endl;
  }

  db->executeQuery("SELECT * FROM products_info");
  std::cout << "Database: " << std::endl;

  auto result = Database::parseQueryToUmap(std::unordered_map<int, std::shared_ptr<Product>>(), db->fetchQuery());

  for (auto it = result.cbegin(); it != result.cend(); it++)
    std::cout << it->second->str() << std::endl;
}
*/

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