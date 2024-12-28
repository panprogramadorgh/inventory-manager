#include "utils/manager.hpp"

// Static methods

// FIXME: terminar

// Manager<ProductInfo>::Container Database::extractContainer<ProductInfo>(QueryResult qresult)
// {
//   Container dest;
//   UmappedProduct row;
//   ProductField field;

//   auto cols = qresult.first;
//   auto vals = qresult.second;

//   for (auto it = vals.begin();
//        it != vals.cend() &&
//        std::distance(vals.begin(), it) % cols.size() < cols.size();
//        it++)
//   {
//     field = string_to_product_field.at(cols[std::distance(vals.begin(), it) % cols.size()]);
//     row[field] = *it;

//     // En caso de que sea la ulima columna por registro
//     if (std::distance(vals.begin(), it) % cols.size() == cols.size() - 1)
//     {
//       // Creates a non-virtual ProductInfo
//       ProductInfo pinfo(row, false);
//       dest.emplace(pinfo.product_id, std::make_shared<ProductInfo>(pinfo));
//       row.clear(); // Limpiar el mapa para la proxima fila
//     }
//   }
//   return dest;
// }

// Manager<ProductInfo>::Container Database::extractContainer<ProductInfo>(QueryResult qresult)
// {
//   Container dest;
//   UmappedProduct row;
//   ProductField field;

//   auto cols = qresult.first;
//   auto vals = qresult.second;

//   for (auto it = vals.begin();
//        it != vals.cend() &&
//        std::distance(vals.begin(), it) % cols.size() < cols.size();
//        it++)
//   {
//     field = string_to_product_field.at(cols[std::distance(vals.begin(), it) % cols.size()]);
//     row[field] = *it;

//     // En caso de que sea la ulima columna por registro
//     if (std::distance(vals.begin(), it) % cols.size() == cols.size() - 1)
//     {
//       // Creates a non-virtual Product
//       ProductInfo pinfo(row, false);
//       dest.emplace(pinfo.product_id, std::make_shared<Product>(Product(pinfo)));
//       row.clear(); // Limpiar el mapa para la proxima fila
//     }
//   }
//   return dest;
// }