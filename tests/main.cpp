#include "forwarder.hpp"
#include "product/product.hpp"
#include "product/smart-product.hpp"

int main()
{
  for (auto &pair : SmartProduct::field_to_string)
  {
    std::cout << pair.first << " -> " << pair.second << std::endl;
  }

  return EXIT_SUCCESS;
}
