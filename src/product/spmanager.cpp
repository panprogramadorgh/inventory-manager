#include "product/spmanager.cpp"

using Spm = SmartProductManager;

template <typename T>
using Sec = Manager<SmartProduct>::SecureReturn<T>;

Sec<SmartProduct> Spm::secGetSmartProduct(std::uint64_t id)
{
  // try
  // {
  //   db->executeQuery(
  //       "SELECT * FROM smart_products_info as p WHERE p.id = $", {std::to_string(id)});
  //   // TODO: Adicionalmente se ha de ejecutar la rutina de estado de conexion y debe combinarse adecuadamente con extractContainer para que la funcion sea lo mas generica posible.
  //   // auto cont = extractContainer(db->fetchQuery());
  //   auto it = cont.find(id);

  //   /* Lo agrega a la cache */
  //   addCache(it->second);

  //   return std::make_pair(std::optional<SmartProduct>(it->second), "");
  // }
  // catch (const std::exception &e)
  // {
  //   return std::make_pair(std::nullopt, e.what());
  // }

  return std::make_pair(std::nullopt, "");
}
