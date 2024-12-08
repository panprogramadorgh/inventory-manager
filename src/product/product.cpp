#include "product/product.hpp"
#include <cassert>

extern UmappedProduct product_field_to_string;

/* Products info class */

std::string ProductInfo::str(std::vector<std::string> visible_fields) const noexcept
{
  auto up = Product::umapProduct(*this); // Representation of ProductInfo on a map
  std::vector<std::string> displayables = std::move(visible_fields);
  std::string csv;

  if (visible_fields.size() < 1)
  {
    assert(displayables.empty()); // core-dumped obtained if displayables does not get empty
    for (auto &field : product_field_to_string)
      displayables.push_back(field.second);
  }

  // Appends the field values as csv
  for (auto &field : displayables)
  {
    if (&displayables[0] != &field)
      csv += ",";
    csv += up.at(string_to_product_field.at(field));
  }
  return csv;
}