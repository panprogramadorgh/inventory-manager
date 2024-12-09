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
  bool should_free_db_ptr;

  QueryUmap cached_products;

  // Gestion interna de recursos cacheados
  std::size_t addProductToCache(std::shared_ptr<Product>) noexcept;

  std::size_t removeProductFromCache(int id) noexcept;

public:
  ProductManager(std::string dbfile)
      : db(nullptr), should_free_db_ptr(true)
  {
    db = new Database(dbfile);
    db->connect();
  }

  // Must receive a connected database (Database::connect())
  ProductManager(Database *db, bool should_manage_mem = false)
      : db(db), should_free_db_ptr(should_manage_mem)
  {
  }

  // TODO: Mejorar Database::connect() abriendo solo conexiones si son inexistentes
  // other.db must be already connected (Database::connect())
  ProductManager(ProductManager &&other)
      : db(other.db),
        should_free_db_ptr(other.should_free_db_ptr),
        cached_products(std::move(other.cached_products))
  {
    other.db = nullptr;
  }

  // Methods

  // Initializes the database with intial data
  ProductManager &init(const std::string &init_file, std::function<void(const Database &, const std::string)> db_initializer)
  {
    db_initializer(*db, init_file);
    return *this;
  }

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

  // Operators

  operator bool()
  {
    return db;
  }

  ProductManager &operator=(ProductManager &&other)
  {
    if (this != &other)
    {
      db = other.db;
      other.db = nullptr;
      should_free_db_ptr = other.should_free_db_ptr;
      cached_products = std::move(other.cached_products);
    }
    return *this;
  }

  ~ProductManager()
  {
    if (should_free_db_ptr)
      delete db;
    db = nullptr;
    // Cached products is managed by its own implementation
  }
};

#endif