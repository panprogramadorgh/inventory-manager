#include "forwarder.hpp"

template <std::uint16_t V>
class ManagerItem
{
public:
  // types

  /* Utilizado para identificador en el mapa de registros */
  template <std::uint8_t V>
  struct RecordField
  {
    static constexpr std::uint8_t value = V;
  };
  /* Alias para tipo subyacente del miembro estatico de RecordField */
  using RecordField_t = decltype(RecordField<0>::value);
  /* Constante de compilacion atajo para RecordField */
  template <std::uint8_t V>
  static constexpr std::uint8_t RecordField_v = RecordField<V>::value;

  /* Tipos empleados para mapas de tablas SQL */
  using RecordUmap = const umap<decltype(RecordField_v<0>), std::string>;
  using ReRecordUmap = const umap<std::string, decltype(RecordField_v<0>)>;

private:
  std::uint16_t cache_rel;

public:
  constexpr std::uint16_t init_cache_rel = V;

  virtual ManagerItem()
      : cache_relevance(init_cache_rel)
  {
  }

  virtual ManagerItem(const ManagerItem &other)
  {
    cache_rel = other.cache_rel;
  }

  virtual ManagerItem(ManagerItem &&other)
  {
    cache_rel = other.cache_rel;
    other.cache_rel = 0;
  }

  /* Obtener relevancia en cache */
  std::uint16_t getCacheRel()
  {
    return cache_rel;
  }

  /* Incrementar relevancia en cache */
  std::uint16_t incCacheRel()
  {
    return ++cache_rel;
  }

  /* Decrementar relevancia en cache */
  std::uint16_t decCacheRel()
  {
    return --cache_rel;
  }

  virtual ~ManagerItem()
  {
    cache_rel = 0;
  }
};