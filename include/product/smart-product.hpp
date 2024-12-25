#include "forwarder.hpp"
#include <netinet/in.h> // Estructuras de direcciones des sockets
#include <arpa/inet.h>  // Formato de direcciones IP en cadena a Big-Endian
#include <sys/socket.h> // Crear descriptor de archivo del servidor (socket TCP)

/* Almacena todos los puertos que emplea para verificar si la maquina remota esta activa */
extern std::vector<uint16_t> liveness_check_ports;

class SmartProduct
{
public:
  // Types

private:
  std::string addr;

public:
  // Verifica si el producto inteligente esta activo
  bool SmartProduct::check_liveness()
}