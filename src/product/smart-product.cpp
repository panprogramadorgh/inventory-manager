#include "product/smart-product.hpp"

using M = ManagerItem;
using Spb = SmartProductBase;
using Sp = SmartProduct;

// Inicializacion de miembros staticos constantes

const M::RecordUmap Spb::field_to_string{
    {static_cast<M::RecordField>(Spb::Rfn::inaddr), std::string("inaddr")}};

const M::ReRecordUmap Spb::string_to_field{
    {std::string("inaddr"), static_cast<M::RecordField>(Spb::Rfn::inaddr)}};

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
    if (target_fd = socket(AF_INET, SOCK_STREAM, 0) < 0)
      return false;
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, inaddr.c_str(), &target_addr.sin_addr) <= 0)
      return false;
    // - - - - - - -

    /* Fallo al establecer la conexion TCP a modo cliente en el puerto determinado */
    if (!connect(target_fd, reinterpret_cast<sockaddr *>(&target_addr), sizeof(target_addr)))
    {
      close(target_fd);
      continue;
    }

    // Premio
    close(target_fd);
    return true;
  }

  // Verifico todos los puertos de la maquina abriendos diferentes sockets pero no encontro nada
  close(target_fd);
  return false;
}

// Miembros estaticos constantes de clase derivada

const M::RecordUmap Sp::field_to_string{
    {static_cast<M::RecordField>(Sp::Rfn::is_active), std::string("is_active")}};

const M::ReRecordUmap Sp::string_to_field{
    {std::string("is_active"), static_cast<M::RecordField>(Sp::Rfn::is_active)}};