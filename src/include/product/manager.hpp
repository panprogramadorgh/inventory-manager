#ifndef PMANAGER_HPP
#define PMANAGER_HPP

#include "../database/interface.hpp"
#include "../database/errors.hpp"
#include "../product/product.hpp"

class ProductManager
{
private:
  Database *db;
  std::unordered_map<int, Product> cached_products;

  // Gestion interna de recursos cacheados
  std::size_t addProductToCache(const Product &p) noexcept;

  std::size_t moveProductToCache(const Product &p) noexcept;

  std::size_t removeProductFromCache(int id) noexcept;

public:
  ProductManager(Database *db = nullptr);

  ProductManager(const ProductManager &other);

  ProductManager &init(std::string init_file, std::function<void(Database &, std::string)> db_initializer);

  /// @brief  Permite obtener prodctos desde la base de datos y los cachea
  /// @param id Identificador del producto dentro de la base de datos
  /// @return Retorna un puntero a un bloque de memoria dinamica que ha de ser manualmente eliminado con delete para llamar al destructor de Product
  const Product *getProduct(int id) noexcept;

  /// @brief Permite agregar productos actualizando la base de datos.
  /// @param p En realidad el miembro `vendor_name` del objeto p es ignorado puesto que el vendedor es indicado como un segundo argumento del metodo.
  /// @param vendor_id Hace referencia al vendedor del producto
  /// @returns Retorna una referencia al nuevo producto insertado (con el nombre del vendedor correspondiente al id indicado)
  Product &addProduct(Product &&p, int vendor_id) noexcept(false);

  /// @brief Permite agregar productos actualizando la base de datos.
  /// @param p En realidad el miembro `vendor_name` del objeto p es ignorado puesto que el vendedor es indicado como un segundo argumento del metodo.
  /// @param vendor_id Hace referencia al vendedor del producto
  /// @returns Retorna una referencia al nuevo producto insertado (con el nombre del vendedor correspondiente al id indicado)
  Product &addProduct(Product &p, int vendor_id) noexcept(false);

  /// @brief Su proposito es eliminar productos de la base de datos y de la cache
  /// @param id Identificador de producto
  void removeProduct(int id) noexcept(false);

  /// @brief  Su proposito es eliminar productos obtenidos con getProduct()
  /// @param p Puntero al producto obtenido desde getProduct()
  void removeProduct(const Product *p) noexcept(false);

  /// @brief Used for debugging propouses
  void foo() noexcept(false);
};

#endif