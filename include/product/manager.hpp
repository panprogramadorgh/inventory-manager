#ifndef PMANAGER_HPP
#define PMANAGER_HPP

#include "database/interface.hpp"
#include "database/dberror.hpp"
#include "product/product.hpp"
#include <optional>
#include <memory>

class ProductManager
{
private:
  Database *db;
  bool should_free_db_ptr;

  QueryUmap<Product> cached_products;

  // Gestion interna de recursos cacheados
  std::size_t addProductToCache(std::shared_ptr<Product>) noexcept;

  std::size_t removeProductFromCache(int id) noexcept;

  // Static members

  struct ErrorMessages
  {
    static constexpr char DB_IS_NULL[] = "Database is null";
    static constexpr char PRODUCT_NOT_FOUND[] = "Product was not found";
    static constexpr char DELETE_PRODUCT_FAILED[] = "Could not delete product";
  };

public:
  template <typename T>
  using SecureReturn = std::pair<std::optional<T>, std::string>;

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
      throw std::runtime_error(ErrorMessages::DB_IS_NULL);
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

  SecureReturn<ProductInfo> secAddProduct(const ProductInfo &p, const int vendor_id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true)) noexcept;
  ProductInfo addProduct(const ProductInfo &p, const int vendor_id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true));

  //

  SecureReturn<int> secRemoveProduct(const int id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true)) noexcept;
  int removeProduct(const int id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true));

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