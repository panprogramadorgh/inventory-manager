#ifndef PMANAGER_HPP
#define PMANAGER_HPP

#include "database/dbinterface.hpp"
#include "product/product.hpp"
#include "utils/manager.hpp"
#include <memory>

class ProductManager : public Manager<Product<true>>
{
private:
  /* Mensajes de error genericos */
  struct ErrMsgs
  {
    static constexpr char PRODUCT_NOT_FOUND[] = "Product was not found";
    static constexpr char DELETE_PRODUCT_FAILED[] = "Could not delete product";
    static constexpr char ADD_PRODUCT_FAILED[] = "Could not add product count";
  };

public:
  ProductManager(const std::string dbfile) : Manager(dbfile)
  {
  }

  // Must receive a connected database (Database::connect())
  ProductManager(Database *db, const bool should_free_db = false)
      : Manager(db, should_free_db)
  {
  }

  ProductManager(const ProductManager &other) = delete;

  // other.db must be already connected (Database::connect())
  ProductManager(ProductManager &&other)
      : Manager(std::move(other))
  {
  }

  // Metodos definidos en archivos de implementacion

  /* Obtencion segura */
  SecureReturn<Product<true>> secGetProduct(const std::uint64_t id) noexcept;
  /* Obtencion normal */
  Product<true> getProduct(const std::uint64_t id);

  /* Creacion segura */
  SecureReturn<Product<true>> secCreateProduct(const Product<false> &p, const std::tuple<bool, bool> hanle_tran) const noexcept;
  /* Creacion normal */
  Product<true> createProduct(const Product<false> &p, const std::tuple<bool, bool> hanle_tran) const;

  /* Adicion segura */
  SecureReturn<std::uint64_t> secAddProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran) noexcept;
  /* Adicion normal */
  std::uint64_t addProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran);

  /* Eliminacion segura */
  SecureReturn<std::uint64_t> secRemoveProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran) noexcept;
  /* Eliminacion normal */
  std::uint64_t removeProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran);

  ~ProductManager() = default;
};

#endif