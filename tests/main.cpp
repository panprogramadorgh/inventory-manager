#include "forwarder.hpp"
#include "product/spmanager.hpp"

int main()
{
  SmartProductManager spm(DB_DATA_FILE_DESTINATION);

  auto secReturn = spm.secGetSmartProduct(1); // This is a dummy call
  if (secReturn.first.has_value())
  {
    std::cout << secReturn.first->toString({}) << std::endl;
  }
  else
  {
    std::cout << "Smart product not found!" << std::endl;
  }

  return EXIT_SUCCESS;
}
