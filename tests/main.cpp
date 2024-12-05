#include <iostream>
#include <string>
#include <vector>

/*

    No hay un problema de implementacon de las funcones. Mas bien error puede estar relacionado corrupcion en un string puesto que falla al momento de llamar std::string::find en cierto momento.

*/

int strInsert(std::string &a, std::string b, std::size_t p, std::size_t n) noexcept
{
  std::string start, end;
  try
  {
    start = a.substr(0, p);
    end = a.substr(p + n);
  }
  catch (const std::exception &)
  {
    return 1;
  }
  return (a = start + b + end), 0;
}

std::string mergeQueryArgs(std::string query, std::vector<std::string> &&args) noexcept
{
  std::size_t pos, i = 0;

  while ((pos = query.find('$')) != std::string::npos)
    strInsert(query, args.at(i++), pos, 1);
  return query;
}

void foo(char *bar)
{
  delete bar;
}

int main()
{
  char *q = "Hola";
  foo(q);
  std::cout << q << std::endl;
}