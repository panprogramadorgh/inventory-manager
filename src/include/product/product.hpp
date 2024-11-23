#ifndef PPRODUCT_HPP
#define PPRODUCT_HPP

#include "../forwarder.hpp"

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

// Maps each sql colum of products_info
inline std::unordered_map<ProductField, std::string> product_field_to_string = {
    {ProductField::product_id, "product_id"},
    {ProductField::product_name, "product_name"},
    {ProductField::product_description, "product_description"},
    {ProductField::vendor_name, "vendor_name"},
    {ProductField::product_count, "product_count"},
    {ProductField::product_price, "product_price"},
};

class ProductInfo
{
public:
  int product_id;
  std::string product_name;
  std::string product_description;
  std::string vendor_name; // sql vendors(vendor_name)
  int product_count;
  double product_price;

  // Constructors
  ProductInfo();
  ProductInfo(int id, std::string name, std::string description, std::string vendor_name, int count = 0, double price = 0.0);
  ProductInfo(const ProductInfo &other) = default; // Copy
  ProductInfo(ProductInfo &&other) noexcept;       // Move

  // Methods
  std::string str() const noexcept;

  // Operators
  ProductInfo &operator=(const ProductInfo &other);
  ProductInfo &operator=(ProductInfo &&other);
};

class Product : private ProductInfo
{
public:
  // Constructors
  Product();
  Product(int id, std::string name, std::string description, std::string vendor_name, int count = 0, double price = 0.0);
  Product(std::unordered_map<std::string, std::string> map);
  Product(const Product &other); // Copy
  Product(Product &&other);      // Move

  // Methods
  int identifier() const noexcept;
  int add(int amount = 1);
  ProductInfo info() const noexcept;
  std::string str() const noexcept;

  // Operators
  Product &operator=(const Product &other);
  Product &operator=(Product &&other);
};

#endif