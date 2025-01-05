#include "forwarder.hpp"
#include "database/interface.hpp"
#include "product/product.hpp"
#include "utils/manager.hpp"
// #include "product/pmanager.hpp"

int main()
{
  // SmartProductManager spm(DB_DATA_FILE_DESTINATION);

  // auto secReturn = spm.secGetSmartProduct(1); // This is a dummy call
  // if (secReturn.first.has_value())
  // {
  //   SmartProduct::prt_full_record_descent(*secReturn.first);
  // }
  // else
  // {
  //   std::cout << secReturn.second << std::endl;
  // }

  // SmartProduct
  //     sp({{static_cast<Rfield>(Brfn::name), "Producto 1"},
  //         {static_cast<Rfield>(Brfn::description), "Descripcion del producto 1"},
  //         {static_cast<Rfield>(Brfn::serial), "XXX"},
  //         {static_cast<Rfield>(Brfn::owner), "Alfredo"},
  //         {static_cast<Rfield>(Brfn::price), std::to_string(14.99)},
  //         {static_cast<Rfield>(SmartProductBase::Rfn::inaddr), "193.168.1.250"},
  //         {static_cast<Rfield>(SmartProduct::Rfn::is_active), std::to_string(1)}},
  //        true);

  // prt_full_record_descent(sp);

  Database db(DB_DATA_FILE_DESTINATION);
  db.connect();

  db.executeQuery("SELECT * FROM products_info");

  // FIXME: This is a dummy call
  auto cont = Manager<Product<true>>::extractContainer(db.fetchQuery());

  // ProductManager::printContainer(cont);

  return EXIT_SUCCESS;
}
