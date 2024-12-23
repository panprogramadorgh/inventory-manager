#ifndef PPRODUCT_HPP
#define PPRODUCT_HPP

#include "forwarder.hpp"

// Ensures secure access to product_field_to_string
enum class ProductField
{
  product_id,
  product_name,
  product_description,
  vendor_name,
  product_count,
  product_price
};

using UmappedProduct = umap<ProductField, std::string>;
using RUmappedProduct = umap<std::string, ProductField>;

// Maps each sql colum of products_info
inline UmappedProduct product_field_to_string = {
    {ProductField::product_id, "product_id"},
    {ProductField::product_name, "product_name"},
    {ProductField::product_description, "product_description"},
    {ProductField::vendor_name, "vendor_name"},
    {ProductField::product_count, "product_count"},
    {ProductField::product_price, "product_price"},
};

inline RUmappedProduct string_to_product_field = {
    {"product_id", ProductField::product_id},
    {"product_name", ProductField::product_name},
    {"product_description", ProductField::product_description},
    {"vendor_name", ProductField::vendor_name},
    {"product_count", ProductField::product_count},
    {"product_price", ProductField::product_price},
};

class ProductInfo
{
public:
  int product_id;
  std::string product_name;
  std::string product_description;
  std::string vendor_name; // vendors(vendor_name)
  int product_count;
  double product_price;

  // Default constructor: Virtual product constructor (id = -1)
  ProductInfo()
      : product_id(-1), product_name(""), product_description(""), vendor_name(""), product_count(0), product_price(0.0)
  {
  }

  // Secure constructor with UmappedProduct
  ProductInfo(const UmappedProduct up, bool is_virtual = true)
      : product_id(-1),
        product_name(up.at(ProductField::product_name)),
        product_description(up.at(ProductField::product_description)),
        vendor_name(up.at(ProductField::vendor_name)),
        product_count(std::atoi(up.at(ProductField::product_count).c_str())),
        product_price(std::atoi(up.at(ProductField::product_price).c_str()))
  {
    if (!is_virtual)
      product_id = std::atoi(up.at(ProductField::product_id).c_str());
  }

  // Constructors
  ProductInfo(ProductInfo &other) noexcept
      : product_id(other.product_id), product_name(other.product_name), product_description(other.product_description), vendor_name(other.vendor_name), product_count(other.product_count), product_price(other.product_price)
  {
  }
  ProductInfo(ProductInfo &&other) noexcept
      : product_id(other.product_id), product_name(std::move(other.product_name)), product_description(std::move(other.product_description)), vendor_name(std::move(other.vendor_name)), product_count(other.product_count), product_price(other.product_price)
  {
    other.product_count = -1;
    other.product_price = -1.0;
  }

  // Methods
  std::string str(std::vector<ProductField> visible_fields = {}) const noexcept;

  bool isVirtual() const noexcept
  {
    return product_id == -1;
  }

  // Operators
  ProductInfo &operator=(ProductInfo &other)
  {
    if (this != &other)
    {
      product_id = other.product_id;
      product_name = other.product_name;
      product_description = other.product_description;
      vendor_name = other.vendor_name;
      product_count = other.product_count;
      product_price = other.product_price;
    }
    return *this;
  }
  ProductInfo &operator=(ProductInfo &&other)
  {
    if (this != &other)
    {
      product_id = other.product_id;
      product_name = std::move(other.product_name);
      product_description = std::move(other.product_description);
      vendor_name = std::move(other.vendor_name);
      product_count = other.product_count;
      product_price = other.product_price;

      other.product_id = -1;
      other.product_count = -1;
      other.product_price = -1.0;
    }
    return *this;
  }

  virtual ~ProductInfo()
  {
    product_id = -1;
    product_count = -1;
    product_price = -1;
  }
};

constexpr int initial_cache_relevance = 10;

class Product : protected ProductInfo
{
private:
  mutable int cache_relevance;

public:
  // Default constructor: Virtual product (id = -1)
  Product()
      : ProductInfo(), cache_relevance(initial_cache_relevance)
  {
  }
  Product(const ProductInfo &pinfo)
      : ProductInfo(
            {
                {ProductField::product_id, std::to_string(pinfo.product_id)},
                {ProductField::product_name, pinfo.product_name},
                {ProductField::product_description, pinfo.product_description},
                {ProductField::vendor_name, pinfo.vendor_name},
                {ProductField::product_count, std::to_string(pinfo.product_count)},
                {ProductField::product_price, std::to_string(pinfo.product_price)},
            },
            pinfo.isVirtual()),
        cache_relevance(initial_cache_relevance)
  {
  }

  Product(const Product &other) = delete;
  Product(Product &&other)
      : ProductInfo(std::move(other)), cache_relevance(initial_cache_relevance)
  {
  }

  // Methods
  int identifier() const noexcept
  {
    return this->product_id;
  }
  ProductInfo info()
  {
    return *this;
  }
  std::string str(std::vector<ProductField> visible_fields = {}) const noexcept
  {
    return ProductInfo::str(visible_fields) + " - " + std::to_string(cache_relevance);
  }

  // Cache methods
  int getCacheRelevance() const noexcept
  {
    return cache_relevance;
  }
  bool decreaseCacheRelevance() const noexcept
  {
    return cache_relevance-- > 0;
  }

  // Operators
  Product &operator=(const Product &other) = delete;
  Product &operator=(Product &&other)
  {
    if (this != &other)
    {
      ProductInfo::operator=(other);
      cache_relevance = other.cache_relevance;
      other.cache_relevance = -1;
    }
    return *this;
  }

  // Static methods
  static UmappedProduct umapProduct(const ProductInfo &p) noexcept
  {
    return {
        {ProductField::product_id, std::to_string(p.product_id)},
        {ProductField::product_name, p.product_name},
        {ProductField::product_description, p.product_description},
        {ProductField::vendor_name, p.vendor_name},
        {ProductField::product_count, std::to_string(p.product_count)},
        {ProductField::product_price, std::to_string(p.product_price)}};
  }

  virtual ~Product()
  {
    cache_relevance = -1;
  }
};

template <typename T>
concept IsPrInfoBased = std::is_base_of_v<ProductInfo, T>;

#endif