#include "utils/manager-item.hpp"

using M = ManagerItem;

// Miembros estaticos y publicos de la clase

// Devuelve un string con los campos seleccionados en formato csv
std::string M::toString(const ManagerItem &m, vec<RecordField> f)
{
  auto record = m.extractRecord();
  auto field_to_string = m.extractRecordBinding().first;
  std::string csv;

  if (f.size() < 1)
  {
    for (auto &field : field_to_string)
      f.push_back(field.first);
  }

  // Appends the field values as csv
  for (auto &field : f)
  {
    if (&f[0] != &field)
      csv += ",";
    csv += record.at(field);
  }
  return csv;
}