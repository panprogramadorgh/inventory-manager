#ifndef PMANAGER_HPP
#define PMANAGER_HPP

#include "database/interface.hpp"
#include "database/dberror.hpp"
#include "product/product.hpp"
#include "utils/interface-manager.hpp"
#include <optional>
#include <memory>

class ProductManager : public Manager<Product>
{
public:
  // Class types
  struct ErrMsgs
  {
    static constexpr char PRODUCT_NOT_FOUND[] = "Product was not found";
    static constexpr char DELETE_PRODUCT_FAILED[] = "Could not delete product";
  };

  // Methods

  SecureReturn<ProductInfo> secGetProduct(const std::size_t id) noexcept;
  ProductInfo getProduct(const std::size_t id);

  //

  SecureReturn<ProductInfo> secAddProduct(const ProductInfo &p, const std::size_t vendor_id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true)) noexcept;
  ProductInfo addProduct(const ProductInfo &p, const std::size_t vendor_id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true));

  //

  SecureReturn<int> secRemoveProduct(const std::size_t id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true)) noexcept;
  int removeProduct(const std::size_t id, const std::tuple<bool, bool> hanle_tran = std::make_tuple(true, true));

  // Inline methods

  std::size_t addCache(std::shared_ptr<Product> p) noexcept override
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

  std::size_t remCache(std::size_t id) noexcept override
  {
    cache.erase(id);
    return cache.size();
  }
};

#endif