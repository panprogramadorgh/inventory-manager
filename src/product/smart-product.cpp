#include "product/smart-product.hpp"

/* Almacena todos los puertos que emplea para verificar si la maquina remota esta activa */
std::vector<uint16_t> liveness_check_ports({135});

// Methods

bool SmartProduct::check_liveness()
{
  sockaddr_in target_addr{};
  int target_fd;

  /* Itera sobre todos los puertos de verificacion de actividad para saber si la maquina esta activa */
  for (const auto &port : liveness_check_ports)
  {
    if (target_fd = socket(AF_INET, SOCK_STREAM, 0) < 0)
      continue;

    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(port);
    /* Formato de direccion de host invalido */
    if (inet_pton(AF_INET, addr.c_str, &target_addr.sin_addr) <= 0)
      continue;

    /* Fallo al establecer la conexion TCP a modo cliente */
    if (!connect(target_fd, (sockaddr *)&target_addr, sizeof(target_addr)))
    {
      close(target_fd);
      continue;
    }

    close(target_fd);
    return true;
  }
}