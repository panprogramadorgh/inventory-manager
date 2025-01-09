#include "utils/manager-item.hpp"

// Type shorthands
using M = ManagerItem;

// Static methods

M::RecordUmap M::forkRecordBinding(const ManagerItem &other, const RecordUmap &new_content)
{
  auto str_binding = other.extractRecordBinding().second;
  auto binding = other.extractRecordBinding().first;

  for (auto &new_field : new_content)
  {
    auto it = binding.find(new_field.first);
    auto it2 = str_binding.find(new_field.second);
    if (it != binding.cend() || it2 != str_binding.cend()) // new_field should have both a new field and a new string
      throw std::invalid_argument("One of the fields already exists in the binding umap");
    binding[new_field.first] = new_field.second;
  }
  return binding;
}

M::ReRecordUmap M::forkRecordBinding(const ManagerItem &other, const ReRecordUmap &new_content)
{
  auto str_binding = other.extractRecordBinding().second;
  auto binding = other.extractRecordBinding().first;

  for (auto &new_field : new_content)
  {
    auto it = binding.find(new_field.second);
    auto it2 = str_binding.find(new_field.first);
    if (it != binding.cend() || it2 != str_binding.cend()) // new_field should have both a new field and a new string
      throw std::invalid_argument("One of the fields already exists in the binding umap");
    str_binding[new_field.first] = new_field.second;
  }
  return str_binding;
}

// Metodos no estaticos

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

  // Table headers
  for (const auto &field : f)
  {
    if (&f[0] != &field)
      csv += ",";
    csv += field_to_string[field];
  }
  csv += "\n";

  // Appends the field values as csv
  for (const auto &field : f)
  {
    if (&f[0] != &field)
      csv += ",";
    csv += record.at(field);
  }
  return csv;
}