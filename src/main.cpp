#include "include/forwarder.hpp"
#include "include/product/manager.hpp"
#include "include/product/product.hpp"
#include "include/utils/dbutils.hpp"
#include "cxxopts.hpp"

#include <fstream>

/* Building configuration */
enum class BuildMode
{
  Release,
  Testing
}

constexpr current_build_mode = BuildMode::Testing;

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

  // TOO: Comprobar integriad de archivo de base de datos por cada vez que se lanza el programa
  // FIXME: Problema al leer archivo de base de datos
  std::ifstream dbfile("/home/alvaro/inventory-manager.db");
  if (!dbfile || !dbfile.good())
  {
    std::cerr << "Colud not read database file" << std::endl;
  }
  else
  {
    std::string line, sql_text;
    while (std::getline(dbfile, line))
      sql_text += line;
    std::cout << sql_text << std::endl;
  }

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
        std::cout << p->info().str(fields) << std::endl;
      else
        throw std::runtime_error("Could not find product with id '" + std::to_string(id) + "'");
    }
    else if (result["method"].as<std::string>() == "rm")
    {
      int id = result["id"].as<int>();
      manager.removeProduct(id); // Throws a QueryError if there is an exception
      std::cout << "Product with id '" << std::to_string(id) << "' was removed" << std::endl;
    }
    else if constexpr (current_build_mode == BuildMode::Testing)
    {
      if (result["method"].as<std::string>() == "init")
      {
        /* Puede tirar: QueryError, InitError, ConnError */
        manager.init(home + "/code/inventory-manager/src/database/init.sql", init_database);
        std::cerr << "Database was initialized" << std::endl;
      }
      else
      {
        throw std::runtime_error("Invalid method");
      }
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