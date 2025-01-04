#include "forwarder.hpp"
#include "database/interface.hpp"
#include "utils/manager.hpp"
#include "product/smart-product.hpp"

class SmartProductManager : public Manager<SmartProductBase>
{
private:
  /* Mensajes de error genericos */
  struct ErrMsgs
  {
    static constexpr char SMART_NOT_FOUND[] = "Smart product was not found";
    static constexpr char DELETE_SMART_FAILED[] = "Could not delete smart product";
    static constexpr char ADD_SMART_FAILED[] = "Could not add smart product count";
  };

public:
  SmartProductManager(const std::string dbfile) : Manager(dbfile)
  {
  }

  // Must receive a connected database (Database::connect())
  SmartProductManager(Database *db, const bool should_free_db = false)
      : Manager(db, should_free_db)
  {
  }

  SmartProductManager(const SmartProductManager &other) = delete;

  // other.db must be already connected (Database::connect())
  SmartProductManager(ProductManager &&other)
      : Manager(std::move(other))
  {
  }

  /* Metodo seguro encargado de obtencion de productos inteligentes. */
  Manager::SecureReturn<SmartProdct> secGetSmartProduct(std::uint64_t id);

  SmartProductManager &operator=(Manager &&other)
  {
    if (this != &other)
    {
      // El operador en la clase base es virtual, por lo que se llama a la implementacion de la clase derivada.

      // Posteriormente probablemente definamos un comportamiento especifico para la clase derivada.
    }
    return *this;
  }
};