#include "forwarder.hpp"
#include "product/product.hpp"
#include <sys/socket.h> // Crear descriptor de archivo del servidor (socket TCP)
#include <netinet/in.h> // Estructuras de direcciones des sockets
#include <arpa/inet.h>  // Formato de direcciones IP en cadena a Big-Endian

class SmartProduct : ProductBase
{
public:
  // Miembros publicos estaticos

  enum class RecordFieldName : RecordField_t
  {
    inaddr = RecordField_v<0>,
  };
  using Rfn = RecordFieldName;

  static RecordUmap field_to_string = {{Rfn::inaddr, "inaddr"}};
  static ReRecordUmap string_to_field = {{"inaddr", Rfn::inaddr}};

  /* Almacena todos los puertos que emplea para verificar si la maquina remota esta activa */
  static vec<std::uint16_t> liveness_check_ports = {
      68, // Client DHCP
      53, // Cliente DNS
      22, // Servidor Secure Shell
      135 // Servidor MSRPC (homogeneidad funciones redes windows)
  };

  // Miembros publicos no estaticos
  std::string inaddr;

  SmartProduct(const RecordUmap record, const bool vrtl)
      : ProductBase(record, vrtl),
        inaddr(field_to_string.at(Rfn::inaddr))
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
  bool SmartProduct::check_liveness();

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
}