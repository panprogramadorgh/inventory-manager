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

  // Metodos virtuales en linea

  // Extracts the record binding
  virtual std::pair<RecordUmap, ReRecordUmap> extractRecordBinding() const noexcept override
  {
    return {field_to_string, string_to_field};
  }

  // Metodos en linea
  virtual RecordUmap extractRecord() const noexcept override
  {
    RecordUmap record = ProductBase::extractRecord();
    record.emplate(static_cast<RecordField>(P_Inaddr), inaddr);

    return record;
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