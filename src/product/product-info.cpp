#include <bits/stdc++.h> // std::find
#include "product/product.hpp"

extern std::unordered_map<ProductField, std::string> product_field_to_string;

ProductInfo::ProductInfo()
    : product_id(-1), product_name(""), product_description(""), vendor_name(""), product_count(0), product_price(0.0)
{
}

ProductInfo::ProductInfo(int id, std::string name, std::string description, std::string vendor_name, int count, double price)
    : product_id(id), product_name(std::move(name)), product_description(std::move(description)), vendor_name(std::move(vendor_name)), product_count(count), product_price(price)
{
}

ProductInfo::ProductInfo(std::string name, std::string description, std::string vendor_name, int count, double price)
    : product_id(-1), product_name(std::move(name)), product_description(std::move(description)), vendor_name(std::move(vendor_name)), product_count(count), product_price(price)
{
}

ProductInfo::ProductInfo(ProductInfo &&other) noexcept
    : product_id(other.product_id), product_name(std::move(other.product_name)), product_description(std::move(other.product_description)), vendor_name(std::move(other.vendor_name)), product_count(other.product_count), product_price(other.product_price)
{
  other.product_count = 0;
  other.product_price = 0.0;
}

std::string ProductInfo::str(const std::vector<std::string> &visible_fields) const noexcept
{
  auto product_umap = Product::parseToUmap(*this); // Representation of an objet on a map
  std::vector<std::string> fields = {
      "product_id",
      "product_name",
      "product_description",
      "vendor_name",
      "product_count",
      "product_price"};
  const bool all_visible = visible_fields[0] == "all";

  std::string result_str;
  for (auto &vfield : (all_visible ? fields : visible_fields))
  {
    // In case is not the first iteration
    if (&(all_visible ? fields : visible_fields)[0] != &vfield)
      result_str += ", ";

    // Appends the field
    result_str += product_umap.at(vfield);
  }

  return "{ " + result_str + " }";
}

ProductInfo &ProductInfo::operator=(const ProductInfo &other)
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

ProductInfo &ProductInfo::operator=(ProductInfo &&other)
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