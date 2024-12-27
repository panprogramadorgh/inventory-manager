#ifndef PPRODUCT_HPP
#define PPRODUCT_HPP

#include "forwarder.hpp"
#include "utils/manager-item.hpp"

constexpr std::uint8_t init_product_cache_relevance = 10;

class ProductBase : public ManagerItem<init_product_cache_relevance>
{
protected:
  bool is_virtual;

public:
  // Miembros estaticos y publicos de la clase

  enum class RecordFieldName : RecordField_t
  {
    id = RecordField_v<0>,
    name = RecordField_v<1>,
    description = RecordField_v<2>,
    serial = RecordField_v<3>,
    owner = RecordField_v<4>,
    vendor_name = RecordField_v<5>,
    price = RecordField_v<6>
  };
  using Rfn = RecordFieldName;

  static constexpr RecordUmap field_to_string = {
      {Rfn::id, "id"},
      {Rfn::name, "name"},
      {Rfn::description, "description"},
      {Rfn::serial, "serial"},
      {Rfn::owner, "owner"},
      {Rfn::vendor_name, "vendor_name"},
      {Rfn::price, "price"}};

  static constexpr ReRecordUmap string_to_field = {
      {"id", Rfn::id},
      {"name", Rfn::name},
      {"description", Rfn::description},
      {"serial", Rfn::serial},
      {"owner", Rfn::owner},
      {"vendor_name", Rfn::vendor_name},
      {"price", Rfn::price}};

  std::uint64_t id;
  std::string name;
  std::string description;
  std::string serial;
  std::string owner;
  std::string vendor_name; // vendors(vendor_name)
  double price;

  // Default constructor: Virtual product constructor (id = -1)
  virtual ProductBase()
      : is_virtual(true)
  {
  }

  // Secure constructor with UmappedProduct
  virtual ProductBase(RecordUmap record, const bool vrtl)
      : is_virtual(vrtl),

        id(record.at(Rfn::id)),
        name(record.at(Rfn::name)),
        description(record.at(Rfn::description)),
        serial(record.at(Rfn::serial)),
        vendor_name(record.at(Rfn::vendor_name)),
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
        vendor_name(other.vendor_name),
        price(other.price)
  {
  }

  virtual ProductBase(ProductBase &&other) noexcept
      : is_virtual(other.is_virtual),

        id(other.id),
        name(std::move(other.name)),
        description(std::move(other.description)),
        serial(std::move(other.serial)),
        vendor_name(std::move(other.vendor_name)),
        price(other.price)
  {
    other.id = 0;
    other.price = 0.0;
  }

  // Metodos publicos en linea
  bool isVirtual()
  {
    return is_virtual;
  }

  // Metodos "interface" de clase base (han de ser definidos en las clases derivadas)
  virtual std::string toString(std::vector<RecordField_t> f = {}) const noexcept = 0;

  // Operators
  virtual ProductBase &operator=(ProductBase &other)
  {
    if (this != &other)
    {
      is_virtual = other.is_virtual;

      id = other.id;
      name = other.name;
      description = other.description;
      vendor_name = other.vendor_name;
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
      vendor_name = std::move(other.vendor_name);
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

class Product : public ProductBase
{
public:
  enum class RecordFieldName : RecordField_t
  {
    id = RecordField_v<0>,
    name = RecordField_v<1>,
    description = RecordField_v<2>,
    serial = RecordField_v<3>,
    owner = RecordField_v<4>,
    vendor_name = RecordField_v<5>,
    price = RecordField_v<6>,
    count = RecordField_v<7>
  };
  using Rfn = RecordFieldName;

  static constexpr RecordUmap field_to_string = {
      {Rfn::id, "id"},
      {Rfn::name, "name"},
      {Rfn::description, "description"},
      {Rfn::serial, "serial"},
      {Rfn::owner, "owner"},
      {Rfn::vendor_name, "vendor_name"},
      {Rfn::price, "price"},
      {Rfn::count, "count"}};

  static constexpr ReRecordUmap string_to_field = {
      {"id", Rfn::id},
      {"name", Rfn::name},
      {"description", Rfn::description},
      {"serial", Rfn::serial},
      {"owner", Rfn::owner},
      {"vendor_name", Rfn::vendor_name},
      {"price", Rfn::price},
      {"count", Rfn::count}};

  // Miembros publicos no estaticos
  std::uint64_t count;

  // Constructores
  virtual ProductInfo()
      : ProductBase(), count(0)
  {
  }

  virtual ProductInfo(RecordUmap record, bool vrtl)
      : ProductBase(record, vrtl), count(record.at(Rfn::count))
  {
  }

  virtual ProductInfo(ProductInfo &other) noexcept
      : ProductBase(other), count(other.count)
  {
  }

  virtual ProductInfo(ProductInfo &&other) noexcept
      : ProductBase(std::move(other)), count(other.count)
  {
    other.price = 0.0;
  }

  // Methods publicos no estaticos en linea
  std::string toString(std::vector<Rfn> f = {}) const noexcept
  {
    auto up = Product::umapProduct(*this); // Representation of ProductInfo on a map
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

  bool isvrtl() const noexcept
  {
    return id == -1;
  }

  // Operators
  ProductInfo &operator=(ProductInfo &other)
  {
    if (this != &other)
      count = other.count;
    return *this;
  }
  ProductInfo &operator=(ProductInfo &&other)
  {
    if (this != &other)
    {
      count = other.count;
      other.count = 0;
    }
    return *this;
  }

  virtual ~ProductInfo()
  {
    count = 0;
  }
};

#endif