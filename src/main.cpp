#include "forwarder.hpp"
#include "product/pmanager.hpp"
#include "product/product.hpp"
#include "cxxopts.hpp"

enum class Method
{
  get,
  create,
  rem
};

umap<Method, std::string> method_to_string = {
    {Method::get, "get"},
    {Method::create, "create"},
    {Method::rem, "rem"},
};

int main(int argc, char **argv)
{
  cxxopts::Options options(argv[0], "Allows to manage inventory with products.");
  options.add_options()("h,help", "Get help")
      // Product properties
      ("id", "Product id", cxxopts::value<int>())
      // * *
      ("n,name", "Product name", cxxopts::value<std::string>())
      // * *
      ("N,show-name", "Shows the product name", cxxopts::value<bool>()->default_value("false"))
      // * *
      ("d,description", "Brief product description", cxxopts::value<std::string>())
      // * *
      ("D,show-description", "Shows the product description", cxxopts::value<bool>()->default_value("false"))
      // Only used when creating a product
      ("v,vendor-id", "Product vendor id", cxxopts::value<int>())
      // * *
      ("V,show-vendor", "Shows product vendor name", cxxopts::value<bool>()->default_value("false"))
      // * *
      ("c,count", "Product quantity", cxxopts::value<int>())
      // * *
      ("C,show-count", "Shows product quantity", cxxopts::value<bool>()->default_value("false"))
      // * *
      ("p,price", "Product value", cxxopts::value<double>())
      // * *
      ("P,show-price", "Shows product value", cxxopts::value<bool>()->default_value("false"))
      // Positionals
      ("method", "Product method", cxxopts::value<std::string>());

  options.allow_unrecognised_options();
  options.parse_positional({"method"});
  auto result = options.parse(argc, argv);

  /* General command line arguments verification */
  if (result.unmatched().size() || !result.count("method"))
  {
    std::cout << options.help() << std::endl;
    return EXIT_FAILURE;
  }
  else if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    return EXIT_SUCCESS;
  }
  /* - - - */

  ProductManager manager(DB_DATA_FILE_DESTINATION);

  try
  {
    /* The `get` method allows the user to display at screen products from the database (at this moment making reference to products just with their id)  */
    if (result["method"].as<std::string>() == method_to_string[Method::get])
    {
      vec<ManagerItem::RecordField> displayables;
      if (result["N"].as<bool>())
        displayables.push_back(static_cast<ManagerItem::RecordField>(ProductBase::Rfn::name));
      if (result["D"].as<bool>())
        displayables.push_back(static_cast<ManagerItem::RecordField>(ProductBase::Rfn::description));
      if (result["V"].as<bool>()) // vendor name
        displayables.push_back(static_cast<ManagerItem::RecordField>(Product<true>::Rfn::vendor_name));
      if (result["C"].as<bool>())
        displayables.push_back(static_cast<ManagerItem::RecordField>(Product<true>::Rfn::count));
      if (result["P"].as<bool>())
        displayables.push_back(static_cast<ManagerItem::RecordField>(ProductBase::Rfn::price));

      if (!result.count("id"))
        throw std::runtime_error(options.help());
      int id = result["id"].as<int>();

      auto product = manager.getProduct(id);
      std::cout << product.toString(displayables) << std::endl;
    }
    /* The `add` method allows users to insert new products in the database. The product id is calculated by the own implementation and must not be specify as a command line option */
    else if (result["method"].as<std::string>() == method_to_string[Method::create])
    {
      /* Product fields */
      std::string name, desc;
      int count, vendor_id;
      double price;
      /* - - - */

      /* Checks for missing command-line options */
      if (!result.count("n") ||
          !result.count("d") ||
          !result.count("vendor-id") ||
          !result.count("c") ||
          !result.count("p"))
      {
        throw std::runtime_error(options.help());
      }
      /* - - - */

      name = result["n"].as<std::string>(), desc = result["d"].as<std::string>();
      count = result["c"].as<int>(), vendor_id = result["vendor-id"].as<int>();
      price = result["p"].as<double>();

      ManagerItem::RecordUmap record{
          {static_cast<ManagerItem::RecordField>(ProductBase::Rfn::name), name},
          {static_cast<ManagerItem::RecordField>(ProductBase::Rfn::description), desc},
          {static_cast<ManagerItem::RecordField>(ProductBase::Rfn::serial), "No serial number"}, // TODO: Terminar
          {static_cast<ManagerItem::RecordField>(ProductBase::Rfn::owner), "No owner"},          // TODO: Terminar
          {static_cast<ManagerItem::RecordField>(Product<false>::Rfn::vendor_id), std::to_string(vendor_id)},
          {static_cast<ManagerItem::RecordField>(Product<false>::Rfn::count), std::to_string(count)},
          {static_cast<ManagerItem::RecordField>(ProductBase::Rfn::price), std::to_string(price)}};
      Product<false> p(record, false);
      /* - - - */

      auto product = manager.createProduct(p, std::make_tuple(true, true));
      std::cout << "New product was created with id '" << product.id << "'" << std::endl;
    }
    else if (result["method"].as<std::string>() == method_to_string[Method::rem])
    {
      /* Checks for missing command-line options */
      if (!result.count("id"))
        throw std::runtime_error(options.help());
      int id = result["id"].as<int>();
      /* - - - */

      manager.removeProduct(id, std::make_tuple(true, true));
      std::cout << "Product with id '" << std::to_string(id) << "' was removed" << std::endl;
    }
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