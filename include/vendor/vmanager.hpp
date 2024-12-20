#include "database/interface.hpp"
#include "utils/interface-manager.hpp"
#include <forwarder.hpp>

using MyType = double;

class VendorManager : public Manager<MyType>
{
public:
  struct ErrMsgs
  {
    static constexpr char TEST[] = "Hello World";
  };
};