#ifndef PMANAGER_HPP
#define PMANAGER_HPP

#include "database/interface.hpp"
#include "database/errors.hpp"
#include "product/product.hpp"
#include <memory>

class ProductManager
{
private:
  Database *db;
  // TODO: Cambiar enfoque de cache a punteros inteligentes. Los errores en tiempo de ejecucion se deben a la eliminacion de las referencias (objetos de memoria de pila) por los std::reference_wrapper
  std::unordered_map<int, std::shared_ptr<Product>> cached_products;

  // Gestion interna de recursos cacheados
  std::size_t addProductToCache(std::shared_ptr<Product>) noexcept;
  
  std::size_t removeProductFromCache(int id) noexcept;

public:
  ProductManager(Database *db = nullptr);

  ProductManager(const ProductManager &other);

  ProductManager &init(const std::string &init_file, std::function<void(const Database &, const std::string)> db_initializer);

  /// @brief  Permite obtener prodctos desde la base de datos y los cachea
  /// @param id Identificador del producto dentro de la base de datos
  /// @return Retorna un puntero a un bloque de memoria dinamica que ha de ser manualmente eliminado con delete para llamar al destructor de Product
  ProductInfo *getProduct(const int id) noexcept;

  /// @brief Permite agregar productos actualizando la base de datos.
  /// @param p En realidad el miembro `vendor_name` del objeto p es ignorado puesto que el vendedor es indicado como un segundo argumento del metodo.
  /// @param vendor_id Hace referencia al vendedor del producto
  /// @returns Retorna una referencia al nuevo producto insertado (con el nombre del vendedor correspondiente al id indicado)
  void addProduct(const ProductInfo &p, const int vendor_id, const bool commit_update = false) noexcept(false);

  /// @brief Su proposito es eliminar productos de la base de datos y de la cache
  /// @param id Identificador de producto
  void removeProduct(const int id, const bool commit_update = true) noexcept(false);
};

#endif