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
      ("id", "Product id", cxxopts::value<int>())
      //
      ("n,name", "Product name", cxxopts::value<std::string>())
      //
      ("N,show-name", "Shows the product name", cxxopts::value<bool>()->default_value("false"))
      //
      ("d,description", "Brief product description", cxxopts::value<std::string>())
      //
      ("D,show-description", "Shows the product description", cxxopts::value<bool>()->default_value("false"))
      // Only used when creating a product
      ("v,vendor-id", "Product vendor id", cxxopts::value<int>())
      //
      ("V,show-vendor", "Shows product vendor name", cxxopts::value<bool>()->default_value("false"))
      //
      ("c,count", "Product quantity", cxxopts::value<int>())
      //
      ("C,show-count", "Shows product quantity", cxxopts::value<bool>()->default_value("false"))
      //
      ("p,price", "Product value", cxxopts::value<double>())
      //
      ("P,show-price", "Shows product value", cxxopts::value<bool>()->default_value("false"))
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
      std::vector<ProductField> displayables;
      if (result["N"].as<bool>())
        displayables.push_back(ProductField::product_name);
      if (result["D"].as<bool>())
        displayables.push_back(ProductField::product_description);
      if (result["V"].as<bool>()) // vendor name
        displayables.push_back(ProductField::product_name);
      if (result["C"].as<bool>())
        displayables.push_back(ProductField::product_count);
      if (result["P"].as<bool>())
        displayables.push_back(ProductField::product_price);

      if (!result.count("id"))
      {
        options.help();
        throw std::exception();
      }

      int id = result["id"].as<int>();
      auto p = manager.getProduct(id);

      if (!p)
      {
        throw std::runtime_error(
            "Could not find product with id '" + std::to_string(id) + "'");
      }
      std::cout << p->str(displayables) << std::endl;
    }
    else if (result["method"].as<std::string>() == "add")
    {
      UmappedProduct up = {
          {ProductField::product_name, result["n"].as<std::string>()},
          {ProductField::product_description, result["d"].as<std::string>()},
          {ProductField::vendor_name, ""}, // We specify the vendor by their id
          {ProductField::product_count, std::to_string(result["c"].as<int>())},
          {ProductField::product_price, std::to_string(result["p"].as<double>())}};
      ProductInfo p(up, true); // Virtual ProductInfo to insert in database
      int vendor_id = result["vendor-id"].as<int>();
      // TODO: Imprimir id de nuevo producto
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