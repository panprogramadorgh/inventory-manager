#include "forwarder.hpp"
#include "product/spmanager.hpp"

using Rfield = ManagerItem::RecordField;

int main()
{
  // SmartProductManager spm(DB_DATA_FILE_DESTINATION);

  // auto secReturn = spm.secGetSmartProduct(1); // This is a dummy call
  // if (secReturn.first.has_value())
  // {
  //   std::cout << secReturn.first->toString({}) << std::endl;
  // }
  // else
  // {
  //   std::cout << "Smart product not found!" << std::endl;
  // }

  // Creando un producto con id de vendedor

  Product<false>
      p({{static_cast<Rfield>(ProductBase::Rfn::name), "Producto 1"},
         {static_cast<Rfield>(ProductBase::Rfn::description), "Descripcion del producto 1"},
         {static_cast<Rfield>(ProductBase::Rfn::serial), "XXX"},
         {static_cast<Rfield>(ProductBase::Rfn::owner), "Alfredo"},
         {static_cast<Rfield>(ProductBase::Rfn::price), std::to_string(14.99)},
         {static_cast<Rfield>(Product<false>::Rfn::vendor_id), std::to_string(2)},
         {static_cast<Rfield>(Product<false>::Rfn::count), std::to_string(29)}},
        true);

  ProductBase pb = p;

  std::cout << pb.toString({}) << "," << p.toString({}) << std::endl;

  return EXIT_SUCCESS;
}
