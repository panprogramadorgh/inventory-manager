#include "forwarder.hpp"
#include "product/spmanager.hpp"
#include "product/smart-product.hpp"

int main()
{
  try
  {
    SmartProductManager spm(DB_DATA_FILE_DESTINATION);

    std::uint64_t product_id = 1; // Example product ID
    auto result = spm.secGetSmartProduct(product_id);

    if (result.first)
    {
      std::cout << ManagerItem::toString(*result.first) << std::endl;
    }
    else
    {
      std::cerr << "Error: " << result.second << std::endl;
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}