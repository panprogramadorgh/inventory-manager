#ifndef PPRODUCT_HPP
#define PPRODUCT_HPP

#include "forwarder.hpp"

template <typename T, typename std::enable_if_t<std::is_integral_v<T>, int> = 0>
class ManagerItem
{
private:
  std::uint16_t cache_relevance;

public:
  constexpr std::uint16_t initial_cache_relevance = static_cast<std::uint16_t>(T::value);

  virtual ManagerItem()
      : cache_relevance(initial_cache_relevance)
  {
  }

  virtual ManagerItem(const ManagerItem &other)
  {
    cache_relevance = other.cache_relevance;
  }

  virtual ManagerItem(ManagerItem &&other)
  {
    cache_relevance = other.cache_relevance;
    other.cache_relevance = 0;
  }

  /* Obtener relevancia en cache */
  std::uint16_t getCacheRel()
  {
    return cache_relevance;
  }

  /* Incrementar relevancia en cache */
  std::uint16_t incCacheRel()
  {
    return ++cache_relevance;
  }

  /* Decrementar relevancia en cache */
  std::uint16_t decCacheRel()
  {
    return --cache_relevance;
  }

  virtual ~ManagerItem()
  {
    cache_relevance = 0;
  }
};

struct Field_t
{
  std::uint8_t value;
};

class ProductBaseField
{
  static constexpr Field_t product_id = Field_t{0};
  static constexpr Field_t product_name = Field_t{1};
  static constexpr Field_t product_description = Field_t{2};
  static constexpr Field_t product_serial = Field_t{3};
  static constexpr Field_t vendor_name = Field_t{4};
  static constexpr Field_t product_price = Field_t{5};
};

inline umap<Field_t, std::string> product_base_field_to_string = {
    {ProductBaseField::product_id, "product_id"},
    {ProductBaseField::product_name, "product_name"},
    {ProductBaseField::product_description, "product_description"},
    {ProductBaseField::product_serial, "product_serial"},
    {ProductBaseField::vendor_name, "vendor_name"},
    {ProductBaseField::product_price, "product_price"},
};

inline umap<std::string, Field_t> string_to_product_base_field = {
    {"product_id", ProductBaseField::product_id},
    {"product_name", ProductBaseField::product_name},
    {"product_description", ProductBaseField::product_description},
    {"product_serial", ProductBaseField::product_serial},
    {"vendor_name", ProductBaseField::vendor_name},
    {"product_price", ProductBaseField::product_price},
};

class ProductBase : public ManagerItem
{
public:
  std::uint64_t product_id;
  std::string product_name;
  std::string product_description;
  std::string product_serial;
  std::string vendor_name; // vendors(vendor_name)
  double product_price;

  // Default constructor: Virtual product constructor (id = -1)
  virtual ProductBase()
      : product_id(0), product_name(""), product_description(""), product_serial(""), vendor_name(""), product_price(0.0)
  {
  }

  // Secure constructor with UmappedProduct
  virtual ProductBase(const umap<Field_t, std::string> fields_map, bool is_virtual = true)
      : product_id(0),
        product_name(fields_map.at(ProductField::product_name)),
        product_description(fields_map.at(ProductField::product_description)),
        product_serial(fields_map.at(ProductField::product_serial)),
        vendor_name(fields_map.at(ProductField::vendor_name)),
        product_price(std::atoi(fields_map.at(ProductField::product_price).c_str()))
  {
    if (!is_virtual)
      product_id = std::atoi(fields_map.at(ProductField::product_id).c_str());
  }

  // Constructors
  virtual ProductBase(ProductBase &other) noexcept
      : product_id(other.product_id), product_name(other.product_name), product_description(other.product_description), product_serial(other.product_serial), vendor_name(other.vendor_name), product_price(other.product_price)
  {
  }

  virtual ProductBase(ProductBase &&other) noexcept
      : product_id(other.product_id), product_name(std::move(other.product_name)), product_description(std::move(other.product_description)), product_serial(std::move(other.product_serial)) vendor_name(std::move(other.vendor_name)), product_price(other.product_price)
  {
    other.product_id = 0;
    other.product_price = 0.0;
  }

  std::string str(std::vector<Field_t> visible_fields = {}) const noexcept = 0;
  bool isvrtl() const noexcept = 0;

  // Operators
  virtual ProductInfo &operator=(ProductInfo &other)
  {
    if (this != &other)
    {
      product_id = other.product_id;
      product_name = other.product_name;
      product_description = other.product_description;
      vendor_name = other.vendor_name;
      product_price = other.product_price;
    }
    return *this;
  }

  virtual ProductBase &operator=(ProductBase &&other)
  {
    if (this != &other)
    {
      product_id = other.product_id;
      product_name = std::move(other.product_name);
      product_description = std::move(other.product_description);
      vendor_name = std::move(other.vendor_name);
      product_price = other.product_price;

      other.product_id = 0;
      other.product_price = 0.0;
    }
    return *this;
  }

  virtual ~ProductBase()
  {
    product_id = 0;
    product_price = 0.0;
  }
}

// Ensures secure access to product_field_to_string
class ProductField : public ProductBaseField
{
  static constexpr Field_t product_count = Field_t{6};
};

inline umap<Field_t, std::string>
    string_to_product_field = {
        {ProductField::product_id, "product_id"},
        {ProductField::product_name, "product_name"},
        {ProductField::product_description, "product_description"},
        {ProductField::product_serial, "product_serial"},
        {ProductField::vendor_name, "vendor_name"},
        {ProductField::product_count, "product_count"},
        {ProductField::product_price, "product_price"},
};

class Product : public ProductBase
{
public:
  std::uint64_t product_count;

  virtual ProductInfo()
      : ProductBase(), product_count(0)
  {
  }

  // Secure constructor with UmappedProduct
  virtual ProductInfo(const umap<Field_t, std::string> fields_map, bool is_virtual = true)
      : ProductBase(fields_map, is_virtual), product_count(fields_map.at(ProductField::product_count))
  {
  }

  // Constructors
  virtual ProductInfo(ProductInfo &other) noexcept
      : ProductBase(other), product_count(other.product_count)
  {
  }

  virtual ProductInfo(ProductInfo &&other) noexcept
      : ProductBase(std::move(other)), product_count(other.product_count)
  {
    other.product_price = 0;
  }

  // Methods
  std::string str(std::vector<ProductField> visible_fields = {}) const noexcept
  {
    auto up = Product::umapProduct(*this); // Representation of ProductInfo on a map
    std::string csv;

    if (visible_fields.size() < 1)
    {
      for (auto &field : product_field_to_string)
        visible_fields.push_back(field.first);
    }

    // Appends the field values as csv
    for (auto &field : visible_fields)
    {
      if (&visible_fields[0] != &field)
        csv += ",";
      csv += up.at(field);
    }
    return csv;
  }

  bool isvrtl() const noexcept
  {
    return product_id == -1;
  }

  // Operators
  ProductInfo &operator=(ProductInfo &other)
  {
    if (this != &other)
      product_count = other.product_count;
    return *this;
  }
  ProductInfo &operator=(ProductInfo &&other)
  {
    if (this != &other)
    {
      product_count = other.product_count;
      other.product_count = 0;
    }
    return *this;
  }

  virtual ~ProductInfo()
  {
    product_count = 0;
  }
};

#endif