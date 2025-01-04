#include "forwarder.hpp"
#include "database/interface.hpp"
#include "utils/manager.hpp"
#include "product/smart-product.hpp"

class SmartProductManager : public Manager<SmartProduct>
{
public:
  /* Metodo seguro encargado de obtencion de productos inteligentes. */
  Manager::SecureReturn<SmartProduct> secGetSmartProduct(std::uint64_t id);
};