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

  static const RecordUmap field_to_string;

  static const ReRecordUmap string_to_field;

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
  ProductBase(RecordUmap record, const bool vrtl)
      : id(std::atoi(record.at(static_cast<std::uint64_t>(Rfn::id)).c_str())),
        name(std::string(record.at(static_cast<std::uint64_t>(Rfn::name)))),
        description(std::string(record.at(static_cast<std::uint64_t>(Rfn::description)))),
        serial(std::string(record.at(static_cast<std::uint64_t>(Rfn::serial)))),
        owner(std::string(record.at(static_cast<std::uint64_t>(Rfn::owner)))),
        price(std::atof(record.at(static_cast<std::uint64_t>(Rfn::price)).c_str()))
  {
  }

  // Constructors
  ProductBase(ProductBase &other)
      : id(other.id),
        name(other.name),
        description(other.description),
        serial(other.serial),
        owner(other.owner),
        price(other.price)
  {
  }

  ProductBase(ProductBase &&other)
      : id(other.id),
        name(std::move(other.name)),
        description(std::move(other.description)),
        serial(std::move(other.serial)),
        owner(std::move(other.owner)),
        price(other.price)
  {
    other.id = 0;
    other.price = 0.0;
  }

  // Metodos "interface" han ed ser definidos por clas clases derivadass
  virtual RecordUmap extractRecord() const noexcept = 0;
  virtual std::string toString(vec<Rfn> f = {}) const noexcept = 0;

  // Operators
  virtual ProductBase &operator=(const ProductBase &other) noexcept
  {
    if (this != &other)
    {
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

ManagerItem::RecordUmap ProductBase::field_to_string = {
    {ProductBase::Rfn::id, std::string("id")},
    {ProductBase::Rfn::name, std::string("name")},
    {ProductBase::Rfn::description, std::string("description")},
    {ProductBase::Rfn::serial, std::string("serial")},
    {ProductBase::Rfn::owner, std::string("owner")},
    {ProductBase::Rfn::price, std::string("price")}};

ManagerItem::ReRecordUmap ProductBase::string_to_field = {
    {std::string("id"), ProductBase::Rfn::id},
    {std::string("name"), ProductBase::Rfn::name},
    {std::string("description"), ProductBase::Rfn::description},
    {std::string("serial"), ProductBase::Rfn::serial},
    {std::string("owner"), ProductBase::Rfn::owner},
    {std::string("price"), ProductBase::Rfn::price}};

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
  static const RecordUmap field_to_string;

  static const ReRecordUmap string_to_string;

  std::uint64_t count;
  std::string vendor_name;

  // Constructores
  Product() noexcept
      : ProductBase(), count(0), vendor_name("")
  {
  }

  Product(RecordUmap record, bool vrtl)
      : ProductBase(record, vrtl), count(std::atoi(record.at(static_cast<std::uint64_t>(Rfn::count)).c_str())), vendor_name(std::string(record.at(static_cast<std::uint64_t>(Rfn::vendor_name))))
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

  // FIXME: El mapa tiene claves duplicadas
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

ManagerItem::RecordUmap Product<true>::field_to_string = {
    {Product<true>::Rfn::count, std::string("count")},
    {Product<true>::Rfn::vendor_name, std::string("vendor_name")}};

ManagerItem::ReRecordUmap Product<true>::string_to_field = {
    {Product<true>::Rfn::count, "count"},
    {Product<true>::Rfn::vendor_name},
    "vendor_name"};

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
  static const RecordUmap field_to_string;

  static const ReRecordUmap field_to_string;

  std::uint64_t count;
  std::uint64_t vendor_id;

  // Constructores
  Product() noexcept
      : ProductBase(), count(0), vendor_id(0)
  {
  }

  Product(RecordUmap record, bool vrtl)
      : ProductBase(record, vrtl), count(std::atoi(record.at(static_cast<std::uint64_t>(Rfn::count)).c_str())), vendor_id(std::atoi(record.at(static_cast<std::uint64_t>(Rfn::vendor_id))).c_str()),
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

ManagerItem::RecordUmap Product<false>::field_to_string = {
    {Product<false>::Rfn::vendor_id, std::string("vendor_id")},
    {Product<false>::Rfn::count, std::string("count")}};

ManagerItem::ReRecordUmap Product<false>::string_to_field = {
    {std::string("vendor_id"), Product<false>::Rfn::vendor_id},
    {std::string("count"), Product<false>::Rfn::count}};

#endif
