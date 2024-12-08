#include <forwarder.hpp>
#include <database/interface.hpp>
#include <product/product.hpp>

int main()
{
  Database db(DATABASE_INIT_FILE);
  
  try
  {
    db.connect();
    db.executeQuery("SELECT * FROM products_info");
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  
  auto result = Database::umapQuery(QueryUmap(), db.fetchQuery());
  Database::printQuery(result);

  return 0;
}
