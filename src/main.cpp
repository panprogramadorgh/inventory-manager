#include "include/forwarder.hpp"
#include "include/product/manager.hpp"
#include "include/product/product.hpp"
#include "include/utils/dbutils.hpp"
#include "cxxopts.hpp"

int main(int argc, char **argv)
{
  cxxopts::Options options(argv[0], "Permite llevar una gestion de inventario con productos.");

  options.add_options()("h,help", "Get help")("id", "Product id", cxxopts::value<int>())
      // Positionals
      ("method", "Product method", cxxopts::value<std::vector<std::string>>());

  options.parse_positional({"method"});
  auto result = options.parse(argc, argv);

  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    return EXIT_SUCCESS;
  }

  if (result.count("method"))
  {
    const auto &test = result["method"].as<std::vector<std::string>>();
    if (test[0] == "get")
    {
      std::cout << result["id"].as<int>() << std::endl;
    }
    else
    {
      std::cerr << "Metodo invalido" << std::endl;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }
  else
  {
    std::cerr << "Se debe especificar un metodo" << std::endl;
    return EXIT_FAILURE;
  }

  std::string home = getenv("HOME");
  Database db(home + "/inventory-manager.db");
  ProductManager manager(&db);

  constexpr int product_id = 2;

  try
  {
    manager.init(home + "/code/inventory-manager/src/database/init.sql", init_database);
    auto p = manager.getProduct(product_id);
    if (p != nullptr)
      std::cout << p->info().product_name << std::endl;
    else
      std::cerr << "Could not find product with id: " << product_id << std::endl;
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}