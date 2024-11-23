#include "../include/product/product.hpp"

extern std::unordered_map<ProductField, std::string> product_field_to_string;

ProductInfo::ProductInfo()
    : product_id(-1), product_name(""), product_description(""), vendor_name(""), product_count(0), product_price(0.0)
{
}

ProductInfo::ProductInfo(int id, std::string name, std::string description, std::string vendor_name, int count, double price)
    : product_id(id), product_name(std::move(name)), product_description(std::move(description)), vendor_name(std::move(vendor_name)), product_count(count), product_price(price)
{
}

ProductInfo::ProductInfo(ProductInfo &&other) noexcept
    : product_id(other.product_id), product_name(std::move(other.product_name)), product_description(std::move(other.product_description)), vendor_name(std::move(other.vendor_name)), product_count(other.product_count), product_price(other.product_price)
{
  other.product_count = 0;
  other.product_price = 0.0;
}

inline std::string ProductInfo::str() const noexcept
{
  return (
      "{ " +
      std::to_string(product_id) + ", " +
      product_name + ", " +
      product_description + ", " +
      vendor_name + ", " +
      std::to_string(product_price) + ", " +
      std::to_string(product_count) +
      " }");
}

inline ProductInfo &ProductInfo::operator=(const ProductInfo &other)
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

inline ProductInfo &ProductInfo::operator=(ProductInfo &&other)
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
    other.product_count = 0;
    other.product_price = 0.0;
  }
  return *this;
}