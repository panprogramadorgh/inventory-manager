#include "forwarder.hpp"
#include "product/manager.hpp"
#include "product/product.hpp"
#include "utils/dbutils.hpp"
#include "cxxopts.hpp"

int main(int argc, char **argv)
{
  cxxopts::Options options(argv[0], "Allows to manager inventory with products.");

  options.add_options()("h,help", "Get help")
      // Product properties
      ("id", "Product id", cxxopts::value<int>())("n,name", "Product name", cxxopts::value<std::string>())("d,description", "Brief product description", cxxopts::value<std::string>())("v,vendorid", "Product vendor id", cxxopts::value<int>())("c,count", "Product quantity", cxxopts::value<int>())("p,price", "Product value", cxxopts::value<double>())
      // Product fields to display
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

  // TODO: Crear funcion de gestion de entrada de comandos
  // TODO: Crear modo "console" donde se procesan varias entrada en una misma "sesion" (llamar a la anterior funcion)
  try
  {
    if (result["method"].as<std::string>() == "get")
    {
      int id = result["id"].as<int>();
      auto fields = result["fields"].as<std::vector<std::string>>();

      auto p = manager.getProduct(id);
      if (p != nullptr)
        std::cout << p->str() << std::endl;
      else
        throw std::runtime_error("Could not find product with id '" + std::to_string(id) + "'");
    }
    else if (result["method"].as<std::string>() == "add")
    {
      UmappedProduct up = {
          {ProductField::product_name, result["name"].as<std::string>()},
          {ProductField::product_description, result["description"].as<std::string>()},
          {ProductField::vendor_name, ""}, // We specify the vendor by their id
          {ProductField::product_count, std::to_string(result["count"].as<int>())},
          {ProductField::product_price, std::to_string(result["price"].as<double>())}};
      ProductInfo p(up); // Virtual ProductInfo to insert in database
      int vendor_id = result["vendorid"].as<int>();
      manager.addProduct(p, vendor_id, true);
    }
    else if (result["method"].as<std::string>() == "rem")
    {
      int id = result["id"].as<int>();
      manager.removeProduct(id, true); // Throws a QueryError if there is an exception
      std::cout << "Product with id '" << std::to_string(id) << "' was removed" << std::endl;
    }
#if DEV_MODE
    else if (result["method"].as<std::string>() == "init")
    {
      /* Puede tirar: QueryError, InitError, ConnError */
      manager.init(DATABASE_INIT_FILE, init_database);
      std::cerr << "Database was initialized" << std::endl;
    }
#endif
    else
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