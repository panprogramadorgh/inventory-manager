#include "product/spmanager.hpp"

using Spm = SmartProductManager;

template <typename T>
using Sec = Spm::SecureReturn<T>;

Sec<SmartProduct> Spm::secGetItem(const std::uint64_t id) noexcept
{
  SmartProductBase spb;
  std::string query;
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

      query =
          DatabaseUtils::createQuery(
              sql_table,
              sql_view,
              "SELECT * FROM # as p WHERE p.id = $", {std::to_string(id)});

      db->executeQuery(query);

      // Extrae el contenedor de la consulta
      auto cont = extractContainer(db->fetchQuery());
      it = cont.find(id);
      if (it == cont.cend())
        throw std::runtime_error(ErrMsgs::SMART_NOT_FOUND);

      spb = *(it->second);
    }

    auto baseRecord = spb.extractRecord();
    baseRecord[P_IsActive] = std::to_string(int(spb.checkLiveness()));

    // Agrega el producto base a la cache
    addCache(std::make_shared<SmartProductBase>(spb));

    // Retorna el producto inteligente
    return std::make_pair(std::optional<SmartProduct>(SmartProduct(baseRecord, false);), "");
  }
  catch (const std::exception &e)
  {
    return std::make_pair(std::nullopt, e.what());
  }
}

Sec<SmartProduct> Spm::getItem(const std::uint64_t id) noexcept
{
  SmartProductBase spb;
  std::string query;

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

    query =
        DatabaseUtils::createQuery(
            sql_table,
            sql_view,
            "SELECT * FROM % as WHERE p.id = $", {std::to_string(id)});

    db->executeQuery(query);

    // Extrae el contenedor de la consulta
    auto cont = extractContainer(db->fetchQuery());
    it = cont.find(id);
    if (it == cont.cend())
      throw std::runtime_error(ErrMsgs::ITEM_NOT_FOUND);

    spb = *(it->second);
  }

  auto baseRecord = spb.extractRecord();
  baseRecord[P_IsActive] = std::to_string(int(spb.checkLiveness()));

  // Agrega el producto base a la cache
  addCache(std::make_shared<SmartProductBase>(spb));

  // Retorna el producto inteligente
  return SmartProduct(baseRecord, false);
}
