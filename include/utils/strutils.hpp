#include "forwarder.hpp"

namespace StringUtilities
{
  inline std::string &insert(std::string &a, std::string b, std::uint64_t p, std::uint64_t n) noexcept
  {
    std::string start, end;
    try
    {
      start = a.substr(0, p);
      end = a.substr(p + n);
    }
    catch (const std::exception &)
    {
      return a;
    }
    a = start + b + end;
    return a;
  }
}