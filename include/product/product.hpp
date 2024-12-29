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
  ProductBase() noexcept
      : ManagerItem()
  {
  }

  // Secure constructor with UmappedProduct
  ProductBase(const RecordUmap record, const bool vrtl)
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
  ProductBase(ProductBase &other)
      : is_virtual(other.is_virtual),

        id(other.id),
        name(other.name),
        description(other.description),
        serial(other.serial),
        owner(record.at(Rfn::owner)),
        price(other.price)
  {
  }

  ProductBase(ProductBase &&other)
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

  // Metodos "interface" han ed ser definidos por clas clases derivadass
  virtual RecordUmap extractRecord() const noexcept = 0;
  std::string toString(vec<Rfn> f = {}) const noexcept = 0;

  // Operators
  virtual ProductBase &operator=(const ProductBase &other) noexcept
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

  virtual ProductBase &operator=(ProductBase &&other) noexcept
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

  virtual ~ProductBase() noexcept
  {
    id = 0;
    price = 0.0;
  }
};

/* Cuando `VendorFieldSelector` es verdadero clase incluye el campo vendor_name, de lo contrario vendor_id (producto crudo a ser insertado en la base de datos) */
template <bool VendorFieldSelector>
class Product;

template <>
class Product<true> : public ProductBase
{
public:
  enum class RecordFieldName : RecordField_t
  {
    count = RecordField_v<0>,
    vendor_name = RecordField_v<1>
  };
  using Rfn = RecordFieldName;

  /* Encargados de proporcionar una forma conveniente de identificar las columnas SQL de los registros recuperados */
  static constexpr RecordUmap field_to_string = {
      {Rfn::count, "count"},
      {Rfn::vendor_name, "vendor_name"}};

  static constexpr ReRecordUmap field_to_string = {
      {"count", Rfn::count},
      {"vendor_name", Rfn::vendor_name}};

  std::uint64_t count;
  std::string vendor_name;

  // Constructores
  Product() noexcept
      : ProductBase(), count(0), vendor_name("")
  {
  }

  Product(RecordUmap record, bool vrtl)
      : ProductBase(record, vrtl), count(record.at(Rfn::count)), vendor_name(record.at(Rfn::vendor_name)),
  {
  }

  Product(const Product<true> &other) noexcept
      : ProductBase(other), count(other.count), vendor_name(other.vendor_name)
  {
  }

  Product(Product<true> &&other) noexcept
      : ProductBase(std::move(other)), count(other.count), vendor_name(std::move(other.vendor_name))
  {
    other.price = 0.0;
  }

  // TODO: Terminar metodo de conversion a record
  RecordUmap extractRecord() const noexcept override
  {
    using Brfn = ProductBase::RecordFieldName;
    return {
        {Brfn::id, id},
        {Brfn::name, name},
        {Brfn::description, description},
        {Brfn::serial, serial},
        {Brfn::owner, owner},
        {Brfn::price, price},
        {Rfn::count, count},
        {Rfn::vendor_name, vendor_name},
    };
  }

  // Metodos en linea
  // TODO:: Adaptar a la especializacion de la clase
  std::string toString(vec<Rfn> f = {}) const noexcept
  {
    auto record = extractRecord();
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
      csv += record.at(field);
    }
    return csv;
  }

  // Operators
  Product &operator=(Product<true> &other) noexcept
  {
    if (this != &other)
    {
      count = other.count;
      vendor_name = other.vendor_name;
    }
    return *this;
  }

  Product &operator=(Product<true> &&other) noexcept
  {
    if (this != &other)
    {
      count = other.count;
      vendor_name = std::move(other.vendor_name);

      other.count = 0;
    }
    return *this;
  }

  ~Product() noexcept
  {
    count = 0;
  }
};

template <>
class Product<false> : public ProductBase
{
public:
  enum class RecordFieldName : RecordField_t
  {
    count = RecordField_v<0>,
    vendor_id = RecordField_v<1>
  };
  using Rfn = RecordFieldName;

  /* Encargados de proporcionar una forma conveniente de identificar las columnas SQL de los registros recuperados */
  static constexpr RecordUmap field_to_string = {
      {Rfn::vendor_id, "vendor_id"},
      {Rfn::count, "count"}};

  static constexpr ReRecordUmap field_to_string = {
      {"vendor_id", Rfn::vendor_id},
      {"count", Rfn::count}};

  std::uint64_t count;
  std::uint64_t vendor_id;

  // Constructores
  Product() noexcept
      : ProductBase(), count(0), vendor_id(0)
  {
  }

  Product(RecordUmap record, bool vrtl)
      : ProductBase(record, vrtl), count(record.at(Rfn::count)), vendor_id(std::atoi(record.at(Rfn::vendor_id))),
  {
  }

  Product(const Product<false> &other) noexcept
      : ProductBase(other), count(other.count), vendor_id(other.vendor_id)
  {
  }

  Product(Product<false> &&other) noexcept
      : ProductBase(std::move(other)), count(other.count), vnedor_id(other.vendor_id)
  {
    other.vendor_id = 0;
    other.price = 0.0;
  }

  // Metodos en linea

  // TODO: Terminar metodo de conversion a record
  RecordUmap extractRecord() const noexcept override
  {
    using Brfn = ProductBase::RecordFieldName;
    return {
        {Brfn::id, id},
        {Brfn::name, name},
        {Brfn::description, description},
        {Brfn::serial, serial},
        {Brfn::owner, owner},
        {Brfn::price, price},
        {Rfn::count, count},
        {Rfn::vendor_id, vendor_id},
    };
  }

  // TODO:: Adaptar a la especializacion de la clase
  std::string toString(vec<Rfn> f = {}) const noexcept
  {
    auto record = extractRecord();
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
      csv += record.at(field);
    }
    return csv;
  }

  // Operators
  Product &operator=(Product<false> &other) noexcept
  {
    if (this != &other)
    {
      count = other.count;
      vendor_id = other.vendor_id;
    }
    return *this;
  }

  Product &operator=(Product<false> &&other) noexcept
  {
    if (this != &other)
    {
      count = other.count;
      vendor_id = other.vendor_id;

      other.count = 0;
      vendor_id = 0;
    }
    return *this;
  }

  ~Product() noexcept
  {
    vendor_id = 0;
    count = 0;
  }
};

#endif