#include "product/product.hpp"

extern UmappedProduct product_field_to_string;

/* Products info class */

std::string ProductInfo::str(std::vector<std::string> visible_fields) const noexcept
{
  auto up = Product::umapProduct(*this); // Representation of ProductInfo on a map
  std::string csv;

  if (visible_fields.size() < 1)
  {
    for (auto &field : product_field_to_string)
      visible_fields.push_back(field.second);
  }

  // Appends the field values as csv
  for (auto &field : visible_fields)
  {
    if (&visible_fields[0] != &field)
      csv += ",";
    csv += up.at(string_to_product_field.at(field));
  }
  return csv;
}