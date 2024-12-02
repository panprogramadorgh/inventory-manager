#include "include/forwarder.hpp"
#include "include/product/manager.hpp"
#include "include/product/product.hpp"
#include "include/utils/dbutils.hpp"
#include "cxxopts.hpp"

int main(int argc, char **argv)
{
  cxxopts::Options options(argv[0], "Allows to manager inventory with products.");

  options.add_options()("h,help", "Get help")
      // Product id
      ("id", "Product id", cxxopts::value<int>())
      // Product fields
      ("f,fields", "Product fields to display", cxxopts::value<std::vector<std::string>>()->default_value({"all"}))
      // Positionals
      ("method", "Product method", cxxopts::value<std::string>());

  options.parse_positional({"method"});
  auto result = options.parse(argc, argv);

  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    return EXIT_SUCCESS;
  }

  if (!result.count("method"))
  {
    std::cerr << "A method must be specified" << std::endl;
    return EXIT_FAILURE;
  }

  std::string home = getenv("HOME");
  Database db(home + "/inventory-manager.db");
  ProductManager manager(&db);

  try
  {
    manager.init(home + "/code/inventory-manager/src/database/init.sql", init_database);
  
    if (result["method"].as<std::string>() == "get")
    {
      int id = result["id"].as<int>();
      auto fields = result["fields"].as<std::vector<std::string>>();
      
      auto p = manager.getProduct(id);
      if (p != nullptr)
        std::cout << p->info().str(fields) << std::endl;
      else
        throw std::runtime_error("Could not find product with id: " + std::to_string(id));
    } else
    {
      throw std::runtime_error("Invalid method");
    }
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}