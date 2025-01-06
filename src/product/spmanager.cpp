#include "product/spmanager.hpp"

using Spm = SmartProductManager;
using Sp = SmartProduct;

template <typename T>
using Sec = Manager<Sp>::SecureReturn<T>;

template <typename T>
using Sec = Manager<SmartProduct>::SecureReturn<T>;

// FIXME: I don't know why spb was deleted memberes like if the destructor was called

Sec<SmartProduct> Spm::secGetSmartProduct(std::uint64_t id)
{
  SmartProductBase spb;
  SmartProduct sp;
  try
  {
    auto it = cache.find(id);
    vec<std::uint64_t> garbage;
    if (it != cache.cend())
    {
      // Mueve el producto base encontrado
      spb = *(it->second);

      // Resta relevancia de cache al resto de productos
      for (auto &cache_pair : cache)
      {
        if (cache_pair.first == id)
          continue;
        if (cache_pair.second->decCacheRel() == 0)
          garbage.push_back(cache_pair.first);
      }
      // Elimina los productos irrelevantes
      for (const auto &id : garbage)
        cache.erase(id);
    }
    else
    {
      // Si no se encontro en cache, se busca en la base de datos y se agregara a cache
      db->executeQuery(
          "SELECT * FROM smart_products_info as p WHERE p.id = $", {std::to_string(id)});

      // DEBUG: After that line _Map_base::at exception is thrown
      std::cout << "Turning point : Before extractContainer" << std::endl;

      // Extrae el contenedor de la consulta
      auto cont = extractContainer(db->fetchQuery());
      it = cont.find(id);
      if (it == cont.cend())
        throw std::runtime_error(ErrMsgs::SMART_NOT_FOUND);

      // Mueve el producto base encontrado
      spb = *(it->second);

      // DEBUG: Prints the product base
      // std::cout << ManagerItem::toString(spb) << std::endl;
    }

    // Extrae el registro del producto base para crear un registro de producto inteligente
    auto baseRecord = spb.extractRecord();
    baseRecord[P_IsActive] = std::to_string(int(spb.checkLiveness()));

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
