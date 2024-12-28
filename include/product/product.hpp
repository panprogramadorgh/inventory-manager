#ifndef PPRODUCT_HPP
#define PPRODUCT_HPP

#include "forwarder.hpp"
#include "utils/manager-item.hpp"

class ProductBase : public ManagerItem
{
public:
  // Miembros estaticos y publicos de la clase

  enum class RecordFieldName : RecordField_t
  {
    id = RecordField_v<0>,
    name = RecordField_v<1>,
    description = RecordField_v<2>,
    serial = RecordField_v<3>,
    owner = RecordField_v<4>,
    price = RecordField_v<6>
  };
  using Rfn = RecordFieldName;

  static constexpr RecordUmap field_to_string = {
      {Rfn::id, "id"},
      {Rfn::name, "name"},
      {Rfn::description, "description"},
      {Rfn::serial, "serial"},
      {Rfn::owner, "owner"},
      {Rfn::price, "price"}};

  static constexpr ReRecordUmap string_to_field = {
      {"id", Rfn::id},
      {"name", Rfn::name},
      {"description", Rfn::description},
      {"serial", Rfn::serial},
      {"owner", Rfn::owner},
      {"price", Rfn::price}};

  // Miembros no estaticos

  std::uint64_t id;
  std::string name;
  std::string description;
  std::string serial;
  std::string owner;
  double price;

  // Default constructor: Virtual product constructor (id = -1)
  virtual ProductBase()
      : ManagerItem()
  {
  }

  // Secure constructor with UmappedProduct
  virtual ProductBase(const RecordUmap record, const bool vrtl) override
      : is_virtual(vrtl),

        id(record.at(Rfn::id)),
        name(record.at(Rfn::name)),
        description(record.at(Rfn::description)),
        serial(record.at(Rfn::serial)),
        owner(record.at(Rfn::owner)),
        price(std::atof(record.at(Rfn::price).c_str()))
  {
  }

  // Constructors
  virtual ProductBase(ProductBase &other) noexcept
      : is_virtual(other.is_virtual),

        id(other.id),
        name(other.name),
        description(other.description),
        serial(other.serial),
        owner(record.at(Rfn::owner)),
        price(other.price)
  {
  }

  virtual ProductBase(ProductBase &&other) noexcept
      : is_virtual(other.is_virtual),

        id(other.id),
        name(std::move(other.name)),
        description(std::move(other.description)),
        serial(std::move(other.serial)),
        owner(record.at(Rfn::owner)),
        price(other.price)
  {
    other.id = 0;
    other.price = 0.0;
  }

  // Operators
  virtual ProductBase &operator=(const ProductBase &other)
  {
    if (this != &other)
    {
      is_virtual = other.is_virtual;

      id = other.id;
      name = other.name;
      description = other.description;
      serial = other.serial;
      owner = other.owner;
      price = other.price;
    }
    return *this;
  }

  virtual ProductBase &operator=(ProductBase &&other)
  {
    if (this != &other)
    {
      is_virtual = other.is_virtual;

      id = other.id;
      name = std::move(other.name);
      description = std::move(other.description);
      serial = other.serial;
      owner = other.owner;
      price = other.price;

      other.id = 0;
      other.price = 0.0;
    }
    return *this;
  }

  virtual ~ProductBase()
  {
    id = 0;
    price = 0.0;
  }
};

/* Cuando `VendorFieldSelector` es verdadero clase incluye el campo vendor_name, de lo contrario vendor_id (producto crudo a ser insertado en la base de datos) */
template <bool VendorFieldSelector>
class Product : public ProductBase
{
public:
  // Miembros publicos estaticos
  enum class RecordFieldName : RecordField_t
  {
    count = RecordField_v<0>
  };
  using Rfn = RecordFieldName;

  static constexpr RecordUmap field_to_string = {{Rfn::count, "count"}};

  static constexpr ReRecordUmap string_to_field = {{"count", Rfn::count}};

  // Miembros publicos no estaticos
  std::uint64_t count;

  // Constructores
  Product(std::enable_if_t<VendorFieldSelector, void> * = nullptr)
      : ProductBase(), count(0)
  {
  }

  Product(RecordUmap record, bool vrtl)
      : ProductBase(record, vrtl), count(record.at(Rfn::count))
  {
  }

  Product(const Product &other) noexcept
      : ProductBase(other), count(other.count)
  {
  }

  Product(Product &&other) noexcept
      : ProductBase(std::move(other)), count(other.count)
  {
    other.price = 0.0;
  }

  // Methods publicos no estaticos en linea
  std::string toString(vec<Rfn> f = {}) const noexcept
  {
    auto up = Product::umapProduct(*this); // Representation of Product on a map
    std::string csv;

    if (f.size() < 1)
    {
      for (auto &field : field_to_string)
        f.push_back(field.first);
    }

    // Appends the field values as csv
    for (auto &field : f)
    {
      if (&f[0] != &field)
        csv += ",";
      csv += up.at(field);
    }
    return csv;
  }

  // Operators
  Product &operator=(Product &other)
  {
    if (this != &other)
      count = other.count;
    return *this;
  }
  Product &operator=(Product &&other)
  {
    if (this != &other)
    {
      count = other.count;
      other.count = 0;
    }
    return *this;
  }

  ~Product()
  {
    count = 0;
  }
};

template <>
class Product<true> : public ProductBase
{
  std::string vendor_name;
};

template <>
class Product<false> : public ProductBase
{
  std::uint64_t vendor_id;
};

#endif