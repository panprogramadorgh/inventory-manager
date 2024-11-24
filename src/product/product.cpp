#include "../include/product/product.hpp"

Product::Product()
    : ProductInfo(), cache_relevance(10)
{
}

Product::Product(int id, std::string name, std::string description, std::string vendor_name, int count, double price)
    : ProductInfo(id, name, description, vendor_name, count, price), cache_relevance(10)
{
}

Product::Product(std::unordered_map<std::string, std::string> map)
    : ProductInfo(
          std::atoi(map.at(product_field_to_string[ProductField::product_id]).c_str()),
          map.at(product_field_to_string[ProductField::product_name]),
          map.at(product_field_to_string[ProductField::product_description]),
          map.at(product_field_to_string[ProductField::vendor_name]),
          std::atoi(map.at(product_field_to_string[ProductField::product_count]).c_str()),
          std::atof(map.at(product_field_to_string[ProductField::product_price]).c_str())),
      cache_relevance(10)
{
}

Product::Product(const Product &other)
    : ProductInfo(other), cache_relevance(10)
{
}

Product::Product(Product &&other)
    : ProductInfo(other), cache_relevance(10)
{
}

int Product::identifier() const noexcept
{
  return this->product_id;
}

int Product::getCacheRelevance() const noexcept
{
  return cache_relevance;
}

bool Product::decreaseCacheRelevance() const noexcept
{
  return cache_relevance-- > 0;
}

int Product::add(int amount)
{
  return 0;
}

ProductInfo Product::info() const noexcept
{
  return static_cast<ProductInfo>(*this);
}

std::string Product::str() const noexcept
{
  return (
      "{ " +
      std::to_string(product_id) + ", " +
      product_name + ", " +
      product_description + ", " +
      vendor_name + ", " +
      std::to_string(product_price) + ", " +
      std::to_string(product_count) +
      " }" + " - " +
      std::to_string(cache_relevance));
}

Product &Product::operator=(const Product &other)
{
  if (this != &other)
  {
    ProductInfo::operator=(other);
  }
  return *this;
}

Product &Product::operator=(Product &&other)
{
  if (this != &other)
  {
    ProductInfo::operator=(other);
  }
  return *this;
}