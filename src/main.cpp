#include "forwarder.hpp"
#include "product/manager.hpp"
#include "product/product.hpp"
#include "utils/dbutils.hpp"
#include "cxxopts.hpp"

int main(int argc, char **argv)
{
  cxxopts::Options options(argv[0], "Allows to manage inventory with products.");

  options.add_options()("h,help", "Get help")
      // Product properties
      ("id", "Product id", cxxopts::value<int>())("n,name", "Product name", cxxopts::value<std::string>())("d,description", "Brief product description", cxxopts::value<std::string>())("v,vendorid", "Product vendor id", cxxopts::value<int>())("c,count", "Product quantity", cxxopts::value<int>())("p,price", "Product value", cxxopts::value<double>())
      // Product fields to display
      ("f,fields", "Product fields to display", cxxopts::value<bool>()->default_value("false"))
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
  };

  ProductManager manager(
      std::string(getenv("HOME")) + "/inventory-manager.db");

  // TODO: Crear funcion de gestion de entrada de comandos
  // TODO: Crear modo "console" donde se procesan varias entrada en una misma "sesion" (llamar a la anterior funcion)
  try
  {
    if (result["method"].as<std::string>() == "get")
    {
      int id = result["id"].as<int>();
      auto p = manager.getProduct(id);

      if (!p)
      {
        throw std::runtime_error(
            "Could not find product with id '" + std::to_string(id) + "'");
      }
      if (result.count("fields"))
      {
        std::cout << p->str({"product_id", "product_name", "product_description", "vendor_name", "product_price", "product_count"}) << std::endl;
      }
      else
      {
        std::cout << "Product was added to cache" << std::endl;
      }
    }
    else if (result["method"].as<std::string>() == "add")
    {
      UmappedProduct up = {
          {ProductField::product_name, result["name"].as<std::string>()},
          {ProductField::product_description, result["description"].as<std::string>()},
          {ProductField::vendor_name, ""}, // We specify the vendor by their id
          {ProductField::product_count, std::to_string(result["count"].as<int>())},
          {ProductField::product_price, std::to_string(result["price"].as<double>())}};
      ProductInfo p(up, true); // Virtual ProductInfo to insert in database
      int vendor_id = result["vendorid"].as<int>();
      manager.addProduct(p, vendor_id, true);
    }
    else if (result["method"].as<std::string>() == "rem")
    {
      int id = result["id"].as<int>();
      manager.removeProduct(id, true);
      std::cout << "Product with id '" << std::to_string(id) << "' was removed" << std::endl;
    }
#if DEV_MODE
    // Resets database data
    else if (result["method"].as<std::string>() == "init")
    {
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