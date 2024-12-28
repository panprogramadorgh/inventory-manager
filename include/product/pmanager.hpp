#ifndef PMANAGER_HPP
#define PMANAGER_HPP

#include "database/interface.hpp"
#include "database/dberror.hpp"
#include "product/product.hpp"
#include "utils/manager.hpp"
#include <optional>
#include <memory>

class ProductManager : public Manager<Product>
{
private:
  /* Mensajes de error genericos */
  struct ErrMsgs
  {
    static constexpr char PRODUCT_NOT_FOUND[] = "Product was not found";
    static constexpr char DELETE_PRODUCT_FAILED[] = "Could not delete product";
  };

public:
  // Metodos definidos en archivos de implementacion

  /* Obtencion segura */
  SecureReturn<Product> secGetProduct(const std::uint64_t id) noexcept;
  /* Obtencion normal */
  Product getProduct(const std::uint64_t id);

  /* Creacion segura */
  SecureReturn<Product> secCreateProduct(const Product &p, const std::uint64_t vendor_id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true)) noexcept;
  /* Creacion normal */
  Product createProduct(const Product &p, const std::uint64_t vendor_id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true));

  /* Adicion segura */
  SecureReturn<std::uint64_t> secAddProduct(const std::uint64_t product_id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true)) noexcept;
  /* Adicion normal */
  std::uint64_t addProduct(const std::uint64_t product_id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true));

  /* Eliminacion segura */
  SecureReturn<int> secRemoveProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true)) noexcept;
  /* Eliminacion normal */
  int removeProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true));

  // Metodos en linea

  std::uint64_t addCache(std::shared_ptr<Product> p) noexcept override
  {
    // Do not add product if it is virtual
    if (p->identifier() == -1)
      return 0;

    auto it = cache.find(p->identifier());
    if (it != cache.cend()) // In case the product does not exist in cache, it is been added.
      return 0;

    cache.emplace(p->identifier(), p);
    return cache.size();
  }

  std::uint64_t remCache(std::uint64_t id) noexcept override
  {
    cache.erase(id);
    return cache.size();
  }
};

#endif