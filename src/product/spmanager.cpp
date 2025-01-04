#include "product/spmanager.hpp"

using Spm = SmartProductManager;

template <typename T>
using Sec = Manager<SmartProduct>::SecureReturn<T>;

Sec<SmartProduct> Spm::secGetSmartProduct(std::uint64_t id)
{
  SmartProductBase spb;
  SmartProduct sp;
  bool found_in_cache = false;
  try
  {
    // TODO: Crear un manejador de cache para simplificar esta terea

    auto it = cache.find(id);
    vec<std::uint64_t> garbage;
    if (it != cache.cend())
    {
      found_in_cache = true;
      spb = it->second;
    }
    // Resta relevancia de cache al resto de productos
    for (auto &each_product : cache)
    {
      if (each_product->decCacheRel() == 0)
        garbage.push_back(each_product->id);
    }
    // Elimina los productos irrelevantes
    for (const auto &id : garbage)
      cache.erase(id);

    // Si se encontro en cache, se retorna
    if (found_in_cache)
      return std::make_pair(std::optional<SmartProduct>(spb), "");

    // Si no se encontro en cache, se busca en la base de datos y se agregara a cache
    db->executeQuery(
        "SELECT * FROM smart_products_info as p WHERE p.id = $", {std::to_string(id)});

    // Extrae el contenedor de la consulta
    auto cont = extractContainer(db->fetchQuery());
    it = cont.find(id);
    if (it == cont.cend())
      throw std::runtime_error(ErrMsgs::SMART_NOT_FOUND);

    // Mueve el producto base encontrado
    spb = it->second;

    // Extrae el registro del producto base para crear un registro de producto inteligente
    auto baseRecord = spb.extractRecord();
    baseRecord[static_cast<RecordField>(Sp::Rfn::is_active)] = std::to_string(int(spb.is_active));

    // Crea el producto inteligente y lo mueve
    sp = SmartProduct(baseRecord, false);

    // Agrega el producto base a la cache
    addCache(std::make_shared<SmartProductBase>(spb));

    // Retorna el producto inteligente
    return std::make_pair(std::optional<SmartProduct>(sp), "");
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::nullopt, e.what());
  }
}
