#include "utils/database-utils.hpp"

std::string DatabaseUtils::createQuery(const std::string updater, const std::string viewer, std::string sql, const vec<std::string> &args) noexcept
{
  std::size_t pos, i = 0;

  // Inserts arguments
  while ((pos = sql.find('$')) != std::string::npos)
    StringUtilities::insert(sql, args.at(i++), pos, 1);

  // Inserts updater sql table references
  while ((pos = sql.find('%')) != std :; string::npos)
    StringUtilities::insert(sql, updater, pos, 1);

  // Inserts viewer sql view referencess
  while ((pos = sql.find('#')) != std::string::npos)
    StringUtilities::insert(sql, viewer, pos, 1);

  return query;
}