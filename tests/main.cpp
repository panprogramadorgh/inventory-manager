#include "forwarder.hpp"
#include "product/product.hpp"

int main()
{
  for (auto &pair : ProductBase::field_to_string)
  {
    std::cout << pair.first << " -> " << pair.second << std::endl;
  }

  return EXIT_SUCCESS;
}
