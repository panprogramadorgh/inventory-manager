#ifndef PMANAGER_HPP
#define PMANAGER_HPP

#include "database/interface.hpp"
#include "database/dberror.hpp"
#include "product/product.hpp"
#include <memory>

class ProductManager
{
private:
  Database *db;
  bool should_free_db_ptr;

  QueryUmap cached_products;

  // Gestion interna de recursos cacheados
  std::size_t addProductToCache(std::shared_ptr<Product>) noexcept;

  std::size_t removeProductFromCache(int id) noexcept;

  // Static members

  struct ErrorMessages
  {
    static constexpr char DB_IS_NULL[] = "Database is null";
    static constexpr char PRODUCT_NOT_FOUND[] = "Product was not found";
  };

public:
  template <typename T>
  using SecureReturn = std::pair<std::shared_ptr<T>, std::string>;

  ProductManager(std::string dbfile)
      : db(nullptr), should_free_db_ptr(true)
  {
    db = new Database(dbfile);
    db->connect();
  }

  // Must receive a connected database (Database::connect())
  ProductManager(Database *db, bool should_free_db_ptr = false)
      : db(db), should_free_db_ptr(should_free_db_ptr)
  {
    if (!db)
      throw std::runtime_error()
  }

  // other.db must be already connected (Database::connect())
  ProductManager(ProductManager &&other)
      : db(other.db),
        should_free_db_ptr(other.should_free_db_ptr),
        cached_products(std::move(other.cached_products))
  {
    other.db = nullptr;
  }

  // Methods

  SecureReturn<ProductInfo> secGetProduct(const int id) noexcept;
  ProductInfo getProduct(const int id);

  //

  SecureReturn<ProductInfo> secAddProduct(const ProductInfo &p, const int vendor_id, const bool commit_update = false) noexcept;
  ProductInfo addProduct(const ProductInfo &p, const int vendor_id, const bool commit_update = false);

  //

  SecureReturn<int> secRemoveProduct(const int id, const bool commit_update = true) noexcept;
  int removeProduct(const int id, const bool commit_update = true);

  // Operators

  operator bool() const noexcept
  {
    return db;
  }

  ProductManager &operator=(ProductManager &&other)
  {
    if (this != &other)
    {
      db = other.db;
      other.db = nullptr;
      should_free_db_ptr = other.should_free_db_ptr;
      cached_products = std::move(other.cached_products);
    }
    return *this;
  }

  ~ProductManager()
  {
    if (should_free_db_ptr)
      delete db;
    db = nullptr;
    // Cached products is managed by its own implementation
  }
};

#endif