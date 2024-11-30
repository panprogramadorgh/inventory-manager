#include "include/forwarder.hpp"
#include "include/product/manager.hpp"
#include "include/product/product.hpp"
#include "include/utils/dbutils.hpp"

int main(int argc, char **argv)
{
  std::cout << "Hello World !" << std::endl;

  // Database db("src/database/test.db");
  // ProductManager manager(&db);

  // constexpr int product_id = 12;

  // try
  // {
  //   manager.init("src/database/init.sql", init_database);
  //   auto p = manager.getProduct(product_id);
  //   if (p != nullptr)
  //     std::cout << p->info().product_name << std::endl;
  //   else
  //     std::cerr << "Could not find product with id: " << product_id << std::endl;
  // }
  // catch (std::exception &e)
  // {
  //   std::cerr << e.what() << std::endl;
  //   return EXIT_FAILURE;
  // }

  return EXIT_SUCCESS;
}