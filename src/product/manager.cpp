#include "../include/product/manager.hpp"
#include "../include/database/interface.hpp"

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
  // Retorna el producto si esta cacheado
  auto it = cached_products.find(id);
  if (it != cached_products.end()) // Se encontro
    return &(it->second);

  // Si aun no se inicializo la base de datos
  if (!db)
    return nullptr;

  std::string query = "SELECT * FROM products_info as p WHERE p.product_id = $";
  try
  {
    db->executeQuery(Database::mergeQueryArgs(std::move(query), {std::to_string(id)}));
  }
  catch (const std::exception)
  {
    return nullptr;
  }

  auto products = Database::parseQueryToUmap(std::unordered_map<int, Product>(), db->fetchQuery());

  it = products.find(id);
  if (it == products.end())
    return nullptr;

  // Asignar bloque de memoria dinamica y llamar al constructor movimiento
  Product *p = new Product();
  *p = std::move(it->second);

  moveProductToCache(*p); // Movemos el objeto encontrado desde el mapa a la cache

  return p;
}

Product &ProductManager::addProduct(Product &&p, int vendor_id) noexcept(false)
{
  std::string query = "INSERT INTO products (product_id, product_name, product_description, vendor_id, product_count, product_price) VALUES ($, $, $, $, $, $)";
  db->executeUpdate(Database::mergeQueryArgs(std::move(query), {std::to_string(p.identifier()),
                                                                "\"" + p.info().product_name + "\"",
                                                                "\"" + p.info().product_description + "\"",
                                                                std::to_string(vendor_id),
                                                                std::to_string(p.info().product_price),
                                                                std::to_string(p.info().product_count)}));

  query = "SELECT * FROM products_info as p WHERE p.product_id = $";
  db->executeQuery(Database::mergeQueryArgs(std::move(query), {std::to_string(p.identifier())}));

  auto result = Database::parseQueryToUmap(std::unordered_map<int, Product>(), db->fetchQuery());
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
    std::cout << key.second.str() << std::endl;
  }

  db->executeQuery("SELECT * FROM products_info");
  std::cout << "Database: " << std::endl;

  auto result = Database::parseQueryToUmap(std::unordered_map<int, Product>(), db->fetchQuery());

  for (auto it = result.cbegin(); it != result.cend(); it++)
    std::cout << it->second.str() << std::endl;
}

// Private methods

std::size_t ProductManager::addProductToCache(const Product &p) noexcept
{
  cached_products.emplace(p.identifier(), p);
  return cached_products.size();
}

std::size_t ProductManager::moveProductToCache(const Product &p) noexcept
{
  cached_products.emplace(p.identifier(), p);
  return cached_products.size();
}

std::size_t ProductManager::removeProductFromCache(int id) noexcept
{
  cached_products.erase(id);
  return cached_products.size();
}