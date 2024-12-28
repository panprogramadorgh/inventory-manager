#include "product/spmanager.cpp"

Manager::SecureReturn SmartProductManager::secGetSmartProduct(std::uint64_t smart_product_id)
{
  try
  {
    db->executeQuery(
        "SELECT * FROM smart_products_info as p WHERE p.smart_product_id = $", {std::to_string(smart_product_id)});
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::nullopt, e.what());
  }

  // TODO: Terminar
  // extractContainer(db->fetchQuery());
}
