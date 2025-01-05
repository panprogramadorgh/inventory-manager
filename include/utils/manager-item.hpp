#ifndef MANAGER_ITEM_HPP
#define MANAGER_ITEM_HPP

#include "forwarder.hpp"

class ManagerItem
{
public:
  /* Utilizado en las claves de los registros / binding de registros */
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

  // Metodos virtuales puros "interface"

  // Extracts the record as a RecordUmap (following the record binding)
  virtual RecordUmap extractRecord() const noexcept = 0;

  // Extracts the record binding
  virtual std::pair<RecordUmap, ReRecordUmap> extractRecordBinding() const noexcept = 0;

  // Usefull with derivated classes which needs to initialize its own field_to_string and string_to_field
  virtual RecordUmap forkRecordBinding(const RecordUmap &new_content) const = 0;
  virtual ReRecordUmap forkRecordBinding(const ReRecordUmap &new_content) const = 0;

  // Metodos publicos estaticos

  // Devuelve un string con los campos seleccionados en formato csv
  static std::string toString(const ManagerItem &m, vec<RecordField> f = {});

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

#endif