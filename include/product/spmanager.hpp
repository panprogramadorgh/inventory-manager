#include "database/dbinterface.hpp"
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
  SmartProductManager(SmartProductManager &&other)
      : Manager(std::move(other))
  {
  }

  /* Metodo seguro encargado de obtencion de productos inteligentes. */
  SecureReturn<SmartProduct> secGetSmartProduct(std::uint64_t id);

  ~SmartProductManager() = default;
};