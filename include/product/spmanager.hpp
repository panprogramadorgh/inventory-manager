#include "forwarder.hpp"
#include "database/interface.hpp"
#include "utils/manager.hpp"
#include "product/smart-product.hpp"

class SmartProductManager : public Manager<SmartProduct>
{
  /* Metodo seguro encargado de obtencion de productos inteligentes. */
  Manager::SecureReturn secGetSmartProduct(std::size_t smart_product_id);
}