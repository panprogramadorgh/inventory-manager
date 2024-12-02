#include "../include/product/manager.hpp"
#include "../include/database/interface.hpp"
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
  std::string query = "SELECT * FROM products_info as p WHERE p.product_id = $";
  try
  {
    db->executeQuery(Database::mergeQueryArgs(std::move(query), {std::to_string(id)}));
  }
  catch (const std::exception)
  {
    delete p;
    return nullptr;
  }
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

void ProductManager::addProduct(Product &p, int vendor_id) noexcept(false)
{
  std::string query = "INSERT INTO products (product_id, product_name, product_description, vendor_id, product_count, product_price) VALUES ($, $, $, $, $, $)";
  db->executeUpdate(
      Database::mergeQueryArgs(
          std::move(query), {std::to_string(p.identifier()),
                             "\"" + p.info().product_name + "\"",
                             "\"" + p.info().product_description + "\"",
                             std::to_string(vendor_id),
                             std::to_string(p.info().product_price),
                             std::to_string(p.info().product_count)}));
}

void ProductManager::removeProduct(int id) noexcept(false)
{
  std::string query = "DELETE FROM products as p WHERE p.product_id = $";
  db->executeUpdate(Database::mergeQueryArgs(std::move(query), {std::to_string(id)}));
  removeProductFromCache(id); // Es eliminado de la cache
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
  auto it = cached_products.find(p->identifier());
  if (it == cached_products.cend()) // In case the product does not exist in cache, it is been added.
  {
    cached_products.emplace(p->identifier(), p);
    return cached_products.size();
  }
  return 0;
}

std::size_t ProductManager::removeProductFromCache(int id) noexcept
{
  cached_products.erase(id);
  return cached_products.size();
}