#include "forwarder.hpp"
#include "product/product.hpp"
#include "product/manager.hpp"
#include "utils/dbutils.hpp"

int main()
{
  // std::string dbfile = std::string(getenv("HOME")) + "/inventory-manager.db";
  // Database db(dbfile.c_str());

  // try
  // {
  //   db.connect();
  //   db.executeQuery("SELECT * FROM foo");
  // }
  // catch (const std::exception &e)
  // {
  //   std::cerr << e.what() << '\n';
  //   return EXIT_FAILURE;
  // }
  // catch (const std::exception *e)
  // {
  //   std::cerr << e->what() << '\n';
  //   delete e;
  //   return EXIT_FAILURE;
  // }

  // auto result = Database::umapQuery(QueryUmap(), db.fetchQuery());
  // Database::printQuery(result);

  ProductManager manager(std::string(getenv("HOME")) + "/inventory-manager.db");

  constexpr int id = 2;

  try
  {
    manager.init(DATABASE_INIT_FILE, init_database);
    ProductInfo *p = manager.getProduct(id);
    if (p)
      std::cout << p->str() << std::endl;
    else
      throw std::runtime_error(std::string("Could not find product ") + std::to_string(id));
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
