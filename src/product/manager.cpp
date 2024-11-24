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
  // Decrease cache relevance of products and remove irrelevant products
  std::vector<int> garbage_products;
  for (auto pair : cached_products)
  {
    if (pair.first != id)
    {
      if (!pair.second.get().decreaseCacheRelevance())
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
    return &(it->second.get());

  // Returns nullptr in case the product is not in cache and datbase is not initialized
  if (!db)
    return nullptr;

  // Obtains the product from database
  std::string query = "SELECT * FROM products_info as p WHERE p.product_id = $";
  try
  {
    db->executeQuery(Database::mergeQueryArgs(std::move(query), {std::to_string(id)}));
  }
  catch (const std::exception)
  {
    return nullptr;
  }
  auto products = Database::parseQueryToUmap(std::unordered_map<int, std::reference_wrapper<Product>>(), db->fetchQuery());
  it = products.find(id);
  if (it == products.end())
    return nullptr;

  addProductToCache(it->second.get()); // And finally we push the product to cache

  // If the product was found, it is moved to the heap and retuerned as a ptr.
  // FIXME: Debug
  // std::bad_alloc is obtained at the moment of call it->second.get().info();
  std::cout << "Obtaining product . . ." << std::endl;
  Product *p = new Product();
  *p = std::move(it->second.get());
  return nullptr;
}

Product &ProductManager::addProduct(Product &&p, int vendor_id) noexcept(false)
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

  query = "SELECT * FROM products_info as p WHERE p.product_id = $";
  db->executeQuery(Database::mergeQueryArgs(std::move(query), {std::to_string(p.identifier())}));

  auto result = Database::parseQueryToUmap(std::unordered_map<int, std::reference_wrapper<Product>>(), db->fetchQuery());
  p = result.at(p.identifier());

  return p;
}

Product &ProductManager::addProduct(Product &p, int vendor_id) noexcept(false)
{
  return addProduct(std::move(p), vendor_id);
}

void ProductManager::removeProduct(int id) noexcept(false)
{
  std::string query = "DELETE FROM products as p WHERE p.product_id = $";
  db->executeUpdate(Database::mergeQueryArgs(std::move(query), {std::to_string(id)}));
  removeProductFromCache(id); // Es eliminado de la cache
}

void ProductManager::removeProduct(const Product *p) noexcept(false)
{
  removeProduct(p->identifier());
  delete p;
}

void ProductManager::foo() noexcept(false)
{
  std::cout << "Cache: " << std::endl;
  for (auto key : cached_products)
  {
    // FIXME: Debug
    // std::bad_alloc error at the moment of call key.second.get().info()
    // std::cout << key.second.get().identifier() << std::endl;
    std::cout << "Reading cache . . ." << std::endl;
    std::cout << key.second.get().info().product_id << std::endl;
    std::cout << key.second.get().str() << std::endl;
  }

  db->executeQuery("SELECT * FROM products_info");
  std::cout << "Database: " << std::endl;

  auto result = Database::parseQueryToUmap(std::unordered_map<int, std::reference_wrapper<Product>>(), db->fetchQuery());

  for (auto it = result.cbegin(); it != result.cend(); it++)
    std::cout << it->second.get().str() << std::endl;
}

// Private methods

std::size_t ProductManager::addProductToCache(Product &p) noexcept
{
  auto it = cached_products.find(p.identifier());
  if (it == cached_products.cend()) // In case the product does not exist in cache, it is been added.
  {
    cached_products.emplace(p.identifier(), p);
    return cached_products.size();
  }
  return 0;
}

std::size_t ProductManager::removeProductFromCache(int id) noexcept
{
  cached_products.erase(id);
  return cached_products.size();
}