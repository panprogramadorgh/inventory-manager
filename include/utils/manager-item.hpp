#include "forwarder.hpp"

class ManagerItem
{
public:
  /* Utilizado en las claves de los registros */
  using RecordField = std::uint16_t;

  /* Tipos empleados para mapas de tablas SQL */
  using RecordUmap = umap<RecordField, std::string>;
  using ReRecordUmap = umap<std::string, RecordField>;

protected:
  bool is_virtual;
  static constexpr std::uint16_t init_cache_rel = 10;

  std::uint16_t cache_rel;

public:
  ManagerItem() noexcept
      : cache_rel(init_cache_rel), is_virtual(true)
  {
  }

  ManagerItem(const ManagerItem &other) noexcept
      : cache_rel(other.cache_rel)
  {
  }

  ManagerItem(ManagerItem &&other) noexcept
      : cache_rel(other.cache_rel)
  {
    other.cache_rel = 0;
  }

  // Metodos "interface"
  virtual RecordUmap extractRecord() const noexcept = 0;
  virtual std::string toString(vec<RecordField> f) const noexcept = 0;

  // Metodos publicos en linea
  bool isVirtual() const noexcept
  {
    return is_virtual;
  }

  /* Obtener relevancia en cache */
  std::uint16_t getCacheRel() const noexcept
  {
    return cache_rel;
  }

  /* Incrementar relevancia en cache */
  std::uint16_t incCacheRel() noexcept
  {
    return ++cache_rel;
  }

  /* Decrementar relevancia en cache */
  std::uint16_t decCacheRel() noexcept
  {
    return --cache_rel;
  }

  // Operadores virtuales
  virtual ManagerItem &operator=(const ManagerItem &other) noexcept
  {
    if (this != &other)
    {
      is_virtual = other.is_virtual;
      cache_rel = other.cache_rel;
    }
    return *this;
  }

  virtual ManagerItem &operator=(ManagerItem &&other) noexcept
  {
    if (this != &other)
    {
      is_virtual = other.is_virtual;
      cache_rel = other.cache_rel;

      other.cache_rel = 0;
    }
    return *this;
  }

  virtual ~ManagerItem()
  {
    cache_rel = 0;
  }
};

template <typename T>
concept ManagerItemBased = std::is_base_of_v<ManagerItem, T> && !std::is_same_v<ManagerItem, T>;
