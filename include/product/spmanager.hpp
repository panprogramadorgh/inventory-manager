#include "database/dbinterface.hpp"
#include "utils/manager.hpp"
#include "product/smart-product.hpp"

class SmartProductManager : public Manager<SmartProductBase>
{
public:
  /// @brief Standard constructor
  /// @param dbfile File path to sqlite database file
  /// @param updater Sql table where all ManagerItems are stored as records
  /// @param viewer Sql view which contains all resolved foreign key references (data queried from this view is used to build ManagerItem instances)
  SmartProductManager(const std::string dbfile, const std::string updater, const std::string viewer)
      : Manager(dbfile, updater, viewer)
  {
  }

  /// @brief Constructor from open database instance
  /// @param db Manager database initializer
  /// @param updater
  /// @param viewer
  /// @todo Must receive a connected database (Database::connect()), so find a way to check if database is connected
  SmartProductManager(Database &db, const std::string updater, const std::string viewer)
      : Manager(db, updater, viewer)
  {
  }

  SmartProductManager(const SmartProductManager &other) = delete;

  // other.db must be already connected (Database::connect())
  SmartProductManager(SmartProductManager &&other)
      : Manager(std::move(other))
  {
  }

  virtual SecureReturn<SmartProduct> secGetItem(const std::uint64_t id) noexcept override;

  virtual SmartProduct getItem(const std::uint64_t id) override;

  ~SmartProductManager() = default;
};