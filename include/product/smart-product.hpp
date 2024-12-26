#include "forwarder.hpp"
#include "product/smart-product.hpp"
#include <netinet/in.h> // Estructuras de direcciones des sockets
#include <arpa/inet.h>  // Formato de direcciones IP en cadena a Big-Endian
#include <sys/socket.h> // Crear descriptor de archivo del servidor (socket TCP)

// Ensures secure access to product_field_to_string
class SmartProductField : public ProductBaseField
{
  static constexpr std::uint16_t product_inaddr = 6;
};

umap<Field_t, std::string> smart_product_field_to_string = {
    {SmartProductField::smart_product_id, "smart_product_id"},
    {SmartProductField::smart_product_name, "smart_product_name"},
    {SmartProductField::smart_product_description, "smart_product_description"},
    {SmartProductField::smart_product_serial, "smart_product_serial"},
    {SmartProductField::vendor_name, "vendor_name"},
    {SmartProductField::smart_product_inaddr, "smart_product_inaddr"},
    {SmartProductField::smart_product_price, "smart_product_price"}};

class SmartProduct : ProductBase
{
public:
  /* Almacena todos los puertos que emplea para verificar si la maquina remota esta activa */
  static constexpr std::vector<uint16_t> liveness_check_ports({68, 53, 22, 135});

  std::uint64_t product_id;
  std::string product_name;
  std::string product_description;
  std::string product_serial;
  std::string vendor_name; // vendors(vendor_name)
  std::string product_inaddr;
  double product_price;

  SmartProductField(const UmappedSmartProduct usp, const bool is_virtual)
      : product_id(-1),
        product_name(usp.at(SmartProductField::smart_product_name)),
        product_description(usp.at(SmartProductField::smart_product_description)),
        product_description(usp.at(SmartProductField::smart_product_serial)),
        vendor_name(usp.at(SmartProductField::vendor_name)),
        product_price(std::atoi(usp.at(SmartProductField::smart_product_price).c_str()))
  {
    if (!is_virtual)
      smart_product_id = std::atoi(usp.at(ProductField::smart_product_id).c_str());
  }

  bool SmartProduct::check_liveness();
}