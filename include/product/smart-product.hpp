#include "forwarder.hpp"
#include "product/product.hpp"
#include <unistd.h>
#include <sys/socket.h> // Crear descriptor de archivo del servidor (socket TCP)
#include <netinet/in.h> // Estructuras de direcciones des sockets
#include <arpa/inet.h>  // Formato de direcciones IP en cadena a Big-Endian

class SmartProductBase : public ProductBase
{
public:
  // Miembros publicos estaticos

  enum class RecordFieldName : RecordField
  {
    inaddr = 6,
  };
  using Rfn = RecordFieldName;

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
        inaddr(field_to_string.at(static_cast<RecordField>(Rfn::inaddr)))
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
  // Miembros publicos estaticos

  enum class RecordFieldName : RecordField
  {
    is_active = 7
  };
  using Rfn = RecordFieldName;

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
        is_active(std::stoi(record.at(static_cast<RecordField>(Rfn::is_active))))
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
  RecordUmap extractRecord() const noexcept override
  {
    using Brfn = ProductBase::RecordFieldName;
    return {{static_cast<RecordField>(Brfn::id), std::to_string(id)},
            {static_cast<RecordField>(Brfn::name), name},
            {static_cast<RecordField>(Brfn::description), description},
            {static_cast<RecordField>(Brfn::serial), serial},
            {static_cast<RecordField>(Brfn::owner), owner},
            {static_cast<RecordField>(Brfn::price), std::to_string(price)},
            {static_cast<RecordField>(SmartProductBase::Rfn::inaddr), inaddr},
            {static_cast<RecordField>(Rfn::is_active),
             std::to_string(int(is_active))}};
  };

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