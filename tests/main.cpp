#include "forwarder.hpp"
#include "product/product.hpp"
#include "product/manager.hpp"
#include "utils/dbutils.hpp"

int main()
{
  std::string dbfile = std::string(getenv("HOME")) + "/inventory-manager.db";
  Database db(dbfile.c_str());

  try
  {
    db.connect();
    db.executeQuery("SELECT * FROM foo");
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }
  catch (const std::exception *e)
  {
    std::cerr << e->what() << '\n';
    delete e;
    return EXIT_FAILURE;
  }

  auto result = Database::umapQuery(QueryUmap(), db.fetchQuery());
  Database::printQuery(result);

  return EXIT_SUCCESS;
}
