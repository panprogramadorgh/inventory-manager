#include "utils/manager.hpp"

using MyType = double;

class VendorManager : public Manager<MyType>
{
public:
  struct ErrMsgs
  {
    static constexpr char TEST[] = "Hello World";
  };
};