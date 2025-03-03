#include "product/smart-product.hpp"

using M = ManagerItem;
using Pb = ProductBase;
using Spb = SmartProductBase;
using Sp = SmartProduct;

// Inicializacion de miembros staticos constantes

const M::RecordUmap Spb::field_to_string = M::forkRecordBinding(Pb(), {{P_Inaddr, "inaddr"}, {P_VendorName, "vendor_name"}});

const M::ReRecordUmap Spb::string_to_field = M::forkRecordBinding(Pb(), {{"inaddr", P_Inaddr}, {"vendor_name", P_VendorName}});

/* Almacena todos los puertos que emplea para verificar si la maquina remota esta activa */
const vec<std::uint16_t> Spb::liveness_check_ports{
    22, // Servidor Secure Shell
    135 // Servidor MSRPC (homogeneidad funciones redes windows)
};

// Methods

bool Spb::checkLiveness()
{
  sockaddr_in target_addr{};
  int target_fd;

  /* Itera sobre todos los puertos de verificacion de actividad para saber si la maquina esta activa */
  for (const auto &port : liveness_check_ports)
  {
    // Cualquier intento de configuracion el socket fallido finaliza la ejecucion de la rutina
    if ((target_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      close(target_fd);
      return false;
    }
    // En todos los casos estamos habriendo conexiones con IPv4
    target_addr.sin_family = AF_INET;

    target_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, inaddr.c_str(), &target_addr.sin_addr) <= 0)
    {
      close(target_fd);
      return false;
    }

    /* Fallo al establecer la conexion TCP a modo cliente en el puerto determinado */
    if (connect(target_fd, reinterpret_cast<sockaddr *>(&target_addr), sizeof(target_addr)))
    {
      close(target_fd);
      continue;
    }

    close(target_fd);
    return true;
  }

  // Verifico todos los puertos de la maquina abriendos diferentes sockets pero no encontro nada
  close(target_fd);
  return false;
}

// Miembros estaticos constantes de clase derivada

const M::RecordUmap Sp::field_to_string = M::forkRecordBinding(Spb(), {{P_IsActive, "is_active"}});

const M::ReRecordUmap Sp::string_to_field = M::forkRecordBinding(Spb(), {{"is_active", P_IsActive}});