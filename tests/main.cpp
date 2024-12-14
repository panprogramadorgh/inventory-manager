#include "forwarder.hpp"
#include "product/product.hpp"
#include "product/manager.hpp"

int main(int argc, char **argv)
{
  using ArgumentList = std::vector<std::string>;
  ArgumentList args;

  while (--argc > 0)
    args.push_back(argv[argc]);

  for (const auto &a : args)
    std::cout << a << std::endl;

  return EXIT_SUCCESS;
}
