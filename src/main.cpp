#include "include/forwarder.hpp"
#include "include/product/manager.hpp"
#include "include/utils/dbutils.hpp"

int main(int argc, char **argv)
{
  Database db("src/database/test.db");
  ProductManager manager(&db);

  try
  {
    manager.init("src/database/init.sql", init_database);
    auto p = manager.getProduct(2);
    std::cout << p->str() << std::endl;
    p = manager.getProduct(2);
    std::cout << p->str() << std::endl;
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}