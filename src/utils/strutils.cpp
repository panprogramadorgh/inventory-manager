#include "utils/strutils.hpp"

std::string &strInsert(std::string &a, std::string b, std::size_t p, std::size_t n) noexcept
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
