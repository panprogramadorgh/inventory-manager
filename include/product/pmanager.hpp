#ifndef PMANAGER_HPP
#define PMANAGER_HPP

#include "database/interface.hpp"
#include "database/dberror.hpp"
#include "product/product.hpp"
#include "utils/manager.hpp"
#include <optional>
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
  // Metodos definidos en archivos de implementacion

  /* Obtencion segura */
  SecureReturn<Product<true>> secGetProduct(const std::uint64_t id) noexcept;
  /* Obtencion normal */
  Product<true> getProduct(const std::uint64_t id);

  /* Creacion segura */
  SecureReturn<Product<true>> secCreateProduct(const Product<false> &p, const std::uint64_t vendor_id, const std::tuple<bool, bool> hanle_tran) const noexcept;
  /* Creacion normal */
  Product<true> createProduct(const Product<false> &p, const std::tuple<bool, bool> hanle_tran) const;

  /* Adicion segura */
  SecureReturn<std::uint64_t> secAddProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran) const noexcept;
  /* Adicion normal */
  std::uint64_t addProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran) const;

  /* Eliminacion segura */
  SecureReturn<int> secRemoveProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran) noexcept;
  /* Eliminacion normal */
  int removeProduct(const std::uint64_t id, const std::tuple<bool, bool> hanle_tran);

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