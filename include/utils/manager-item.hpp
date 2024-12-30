#include "forwarder.hpp"

class ManagerItem
{
protected:
  bool is_virtual;

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
  using RecordUmap = const umap<RecordField_t, std::string>;
  using ReRecordUmap = const umap<std::string, RecordField_t>;

  /* Constantes de compilacion "interface compliant" */
  static RecordUmap field_to_string;
  static ReRecordUmap string_to_field;

protected:
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
  virtual std::string toString(vec<RecordField_t> f = {}) const noexcept = 0;

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
