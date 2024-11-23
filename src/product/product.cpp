#include "../include/product/product.hpp"

Product::Product()
    : ProductInfo()
{
}

Product::Product(int id, std::string name, std::string description, std::string vendor_name, int count, double price)
    : ProductInfo(id, name, description, vendor_name, count, price)
{
}

Product::Product(std::unordered_map<std::string, std::string> map)
    : ProductInfo(
          std::atoi(map.at(product_field_to_string[ProductField::product_id]).c_str()),
          map.at(product_field_to_string[ProductField::product_name]),
          map.at(product_field_to_string[ProductField::product_description]),
          map.at(product_field_to_string[ProductField::vendor_name]),
          std::atoi(map.at(product_field_to_string[ProductField::product_count]).c_str()),
          std::atof(map.at(product_field_to_string[ProductField::product_price]).c_str()))
{
}

Product::Product(const Product &other)
    : ProductInfo(other)
{
}

Product::Product(Product &&other)
    : ProductInfo(other)
{
}

std::string Product::str() const noexcept
{
  return ProductInfo::str();
}

int Product::add(int amount)
{
  return 0;
}

int Product::identifier() const noexcept
{
  return this->product_id;
}

ProductInfo Product::info() const noexcept
{
  return static_cast<ProductInfo>(*this);
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