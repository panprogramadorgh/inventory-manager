#include "forwarder.hpp"
#include "product/product.hpp"
#include <unistd.h>
#include <sys/socket.h> // Crear descriptor de archivo del servidor (socket TCP)
#include <netinet/in.h> // Estructuras de direcciones des sockets
#include <arpa/inet.h>  // Formato de direcciones IP en cadena a Big-Endian

class SmartProduct : ProductBase
{
public:
  // Miembros publicos estaticos

  enum class RecordFieldName : RecordField
  {
    inaddr = 6
  };
  using Rfn = RecordFieldName;

  static const RecordUmap field_to_string;
  static const ReRecordUmap string_to_field;

  /* Almacena todos los puertos que emplea para verificar si la maquina remota esta activa */
  static const vec<std::uint16_t> liveness_check_ports;

  // Miembros publicos no estaticos
  std::string inaddr;

  SmartProduct(const RecordUmap record, const bool vrtl)
      : ProductBase(record, vrtl),
        inaddr(field_to_string.at(static_cast<RecordField>(Rfn::inaddr)))
  {
  }

  SmartProduct(const SmartProduct &other)
      : ProductBase(other), inaddr(other.inaddr)
  {
  }

  SmartProduct(SmartProduct &&other)
      : ProductBase(std::move(other)), inaddr(other.inaddr)
  {
  }

  // Metodos publicos en archivos ed implmentacion
  bool checkLiveness();

  RecordUmap extractRecord() const noexcept override
  {
    using Brfn = ProductBase::RecordFieldName;

    return {
        {static_cast<RecordField>(Brfn::id), std::to_string(id)},
        {static_cast<RecordField>(Brfn::name), name},
        {static_cast<RecordField>(Brfn::description), description},
        {static_cast<RecordField>(Brfn::serial), serial},
        {static_cast<RecordField>(Brfn::owner), owner},
        {static_cast<RecordField>(Brfn::price), std::to_string(price)},
        {static_cast<RecordField>(Rfn::inaddr), inaddr}};
  };

  std::string toString(vec<RecordField> f) const noexcept
  {
    auto record = extractRecord();
    std::string csv;

    if (f.size() < 1)
    {
      for (auto &field : field_to_string)
        f.push_back(static_cast<RecordField>(field.first));
    }

    // Appends the field values as csv
    for (auto &field : f)
    {
      if (&f[0] != &field)
        csv += ",";
      csv += record.at(static_cast<RecordField>(field));
    }
    return csv;
  }

  // Operadores

  SmartProduct &operator=(const SmartProduct &other)
  {
    if (this != &other)
    {
      inaddr = other.inaddr;
    }
    return *this;
  }

  SmartProduct &operator=(SmartProduct &&other)
  {
    if (this != &other)
    {
      inaddr = std::move(other.inaddr);
    }
    return *this;
  }

  ~SmartProduct() = default;
};