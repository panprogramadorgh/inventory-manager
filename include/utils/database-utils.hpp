#ifndef DATABASE_UTILS_HPP
#define DATABASE_UTILS_HPP

#include "forwarder.hpp"

namespace DatabaseUtils
{
  /// @brief Allows to create dynamic sql query strings using both updater and viewer
  /// @param updater Is the name of the sql table which stores the corresponding ManagerItem records
  /// @param viewer Is the name of the sql view that resolves al references to foreign keys (thus queries to view are used to build ManagerItems from record)
  /// @param sql Is the sql raw string template (% refers to updater, # is viewer and $ are the arguments)
  /// @param args Represents a list of arguments (std::vector<std::string>)
  /// @return Returns the formatted sql query string
  std::string DatabaseUtils::createQuery(const std::string updater, const std::string viewer, std::string sql, const vec<std::string> &args = {}) noexcept;
};

#endif