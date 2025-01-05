#ifndef PPRODUCT_HPP
#define PPRODUCT_HPP

#include "forwarder.hpp"
#include "utils/manager-item.hpp"

/*
[Change]
  Ahora el acceso a los campos de los registros se realiza con un enum sin tipo evitando los casting explicitos y centralizando en valor para cada campo en n mismo lugar. Para los productos se emplea el prefijo P_ seguido del nombre del campo en PascalCase.
*/

// Enumeracion de campos de registro
enum P_RecordFieldName
{
  // ProductBase
  P_Id = 0,
  P_Name,
  P_Description,
  P_Serial,
  P_Owner,
  P_Price,

  // Product (both specializations)
  P_Count,
  P_VendorName,
  P_VendorId,

  // SmartProduct and SmartProductBase
  P_Inaddr,
  P_IsActive
};

class ProductBase : public ManagerItem
{
public:
  // Record binding for extractRecord calls
  static const RecordUmap field_to_string;
  static const ReRecordUmap string_to_field;

  // BE CAREFUL OUTSIDE, ManagerItems like this may be incorrectly modify within the Manager::Container or derived classes
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
      : ManagerItem(),
        name(record.at(P_Name)),
        description(record.at(P_Description)),
        serial(record.at(P_Serial)),
        owner(record.at(P_Owner)),
        price(std::stold(record.at(P_Price)))
  {
    if (!vrtl)
      id = std::stoull(record.at(P_Id));
  }

  // Constructors
  ProductBase(const ProductBase &other)
      : ManagerItem(other),
        id(other.id),
        name(other.name),
        description(other.description),
        serial(other.serial),
        owner(other.owner),
        price(other.price)
  {
  }

  ProductBase(ProductBase &&other)
      : ManagerItem(std::move(other)),
        id(other.id),
        name(std::move(other.name)),
        description(std::move(other.description)),
        serial(std::move(other.serial)),
        owner(std::move(other.owner)),
        price(other.price)
  {
    other.id = 0;
    other.price = 0.0;
  }

  // Metodos virtuales en linea

  // Extracts the record binding
  virtual std::pair<RecordUmap, ReRecordUmap> extractRecordBinding() const noexcept override
  {
    return {field_to_string, string_to_field};
  }

  // Extracts the record as a RecordUmap (following the record binding)
  virtual RecordUmap extractRecord() const noexcept override
  {
    return {
        {P_Id, std::to_string(id)},
        {P_Name, name},
        {P_Description, description},
        {P_Serial, serial},
        {P_Owner, owner},
        {P_Price, std::to_string(price)}};
  }

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

/* Cuando `VendorFieldSelector` es verdadero clase incluye el campo vendor_name, de lo contrario vendor_id (producto crudo a ser insertado en la base de datos) */
template <bool VendorFieldSelector>
class Product;

template <>
class Product<true> : public ProductBase
{
public:
  // Record binding for extractRecord calls
  static const RecordUmap field_to_string;
  static const ReRecordUmap string_to_field;

  std::uint64_t count;
  std::string vendor_name;

  // Constructores
  Product()
      : ProductBase(), count(0), vendor_name("")
  {
  }

  Product(RecordUmap record, bool vrtl)
      : ProductBase(record, vrtl),
        count(std::stoull(record.at(P_Count))),
        vendor_name(record.at(P_VendorName))
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

  // Extracts the record binding
  std::pair<RecordUmap, ReRecordUmap> extractRecordBinding() const noexcept override
  {
    return {field_to_string, string_to_field};
  }

  RecordUmap extractRecord() const noexcept override
  {
    RecordUmap record = ProductBase::extractRecord();
    record.emplace(P_Count, std::to_string(count));
    record.emplace(P_VendorName, vendor_name);

    return record;
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

  ~Product()
  {
    count = 0;
  }
};

template <>
class Product<false> : public ProductBase
{
public:
  // Record binding for extractRecord calls
  static const RecordUmap field_to_string;
  static const ReRecordUmap string_to_field;

  std::uint64_t count;
  std::uint64_t vendor_id;

  // Constructores
  Product() noexcept
      : ProductBase(), count(0), vendor_id(0)
  {
  }

  Product(RecordUmap record, bool vrtl)
      : ProductBase(record, vrtl),
        count(std::stoull(record.at(P_Count))),
        vendor_id(std::stoull(record.at(P_VendorId)))
  {
  }

  Product(const Product<false> &other) noexcept
      : ProductBase(other), count(other.count), vendor_id(other.vendor_id)
  {
  }

  Product(Product<false> &&other) noexcept
      : ProductBase(std::move(other)), count(other.count), vendor_id(other.vendor_id)
  {
    other.vendor_id = 0;
    other.price = 0.0;
  }

  // Extracts the record binding
  std::pair<RecordUmap, ReRecordUmap> extractRecordBinding() const noexcept override
  {
    return {field_to_string, string_to_field};
  }

  // Metodos en linea
  RecordUmap extractRecord() const noexcept override
  {
    RecordUmap record = ProductBase::extractRecord();
    record.emplace(P_Count, std::to_string(count));
    record.emplace(P_VendorId, std::to_string(vendor_id));

    return record;
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
