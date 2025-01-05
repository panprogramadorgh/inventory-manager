#include "forwarder.hpp"
#include "product/product.hpp"
#include <unistd.h>
#include <sys/socket.h> // Crear descriptor de archivo del servidor (socket TCP)
#include <netinet/in.h> // Estructuras de direcciones des sockets
#include <arpa/inet.h>  // Formato de direcciones IP en cadena a Big-Endian

class SmartProductBase : public ProductBase
{
public:
  // Record binding for extractRecord calls
  static const RecordUmap field_to_string;
  static const ReRecordUmap string_to_field;

  /* Almacena todos los puertos que emplea para verificar si la maquina remota esta activa */
  static const vec<std::uint16_t> liveness_check_ports;

  // Miembros publicos no estaticos
  std::string inaddr;

  SmartProductBase()
      : ProductBase(), inaddr()
  {
  }

  SmartProductBase(const RecordUmap record, const bool vrtl)
      : ProductBase(record, vrtl),
        inaddr(field_to_string.at(P_Inaddr))
  {
  }

  SmartProductBase(const SmartProductBase &other)
      : ProductBase(other), inaddr(other.inaddr)
  {
  }

  SmartProductBase(SmartProductBase &&other)
      : ProductBase(std::move(other)), inaddr(std::move(other.inaddr))
  {
  }

  // Metodos publicos en archivos ed implmentacion
  bool checkLiveness();

  // Metodos en linea
  virtual RecordUmap extractRecord() const noexcept override
  {
    RecordUmap record = ProductBase::extractRecord();
    record.emplate(static_cast<RecordField>(P_Inaddr), inaddr);

    return record;
  };

  // Extracts the record binding
  virtual std::pair<RecordUmap, ReRecordUmap> extractRecordBinding() const noexcept override
  {
    return {field_to_string, string_to_field};
  }

  // Operadores

  virtual SmartProductBase &operator=(const SmartProductBase &other)
  {
    if (this != &other)
    {
      inaddr = other.inaddr;
    }
    return *this;
  }

  virtual SmartProductBase &operator=(SmartProductBase &&other)
  {
    if (this != &other)
    {
      inaddr = std::move(other.inaddr);
    }
    return *this;
  }

  ~SmartProductBase() = default;
};

class SmartProduct : public SmartProductBase
{
public:
  // Record binding for extractRecord calls
  static const RecordUmap field_to_string;
  static const ReRecordUmap string_to_field;

  // Miembros publicos no estaticos
  bool is_active;

  SmartProduct()
      : SmartProductBase(), is_active(false)
  {
  }

  SmartProduct(const RecordUmap record, const bool vrtl)
      : SmartProductBase(record, vrtl),
        is_active(std::stoi(record.at(P_IsActive)))
  {
  }

  SmartProduct(const SmartProduct &other)
      : SmartProductBase(other), is_active(other.is_active)
  {
  }

  SmartProduct(SmartProduct &&other)
      : SmartProductBase(std::move(other)), is_active(other.is_active)
  {
    other.is_active = false;
  }

  // Metodos publicos en archivos ed implmentacion
  bool checkLiveness();

  // Metodos en linea

  virtual RecordUmap extractRecord() const noexcept override
  {
    RecordUmap record = SmartProductBase::extractRecord();
    record.emplate(static_cast<RecordField>(P_IsActive), is_active);

    return record;
  };

  // Extracts the record binding
  virtual std::pair<RecordUmap, ReRecordUmap> extractRecordBinding() const noexcept override
  {
    return {field_to_string, string_to_field};
  }

  // Operadores

  SmartProduct &operator=(const SmartProduct &other)
  {
    if (this != &other)
    {
      is_active = other.is_active;
    }
    return *this;
  }

  SmartProduct &operator=(SmartProduct &&other)
  {
    if (this != &other)
    {
      is_active = other.is_active;
      other.is_active = false;
    }
    return *this;
  }

  ~SmartProduct()
  {
    is_active = false;
  }
};