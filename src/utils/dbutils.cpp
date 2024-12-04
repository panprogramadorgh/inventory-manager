#include "../include/utils/dbutils.hpp"
#include "../include/database/errors.hpp"
#include <fstream>

void init_database(Database &db, std::string db_init_file)
{
  std::ifstream file(db_init_file);
  std::string each_line, sql_text;

  if (!file || !file.good()) // Error leyendo archivo texto sql
    throw InitError(file.rdstate());
  while (std::getline(file, each_line))
    sql_text += each_line + '\n';

  db.executeQuery("BEGIN TRANSACTION");
  db.executeUpdate(sql_text); // Puede lanzar error de consulta
  db.executeQuery("COMMIT");
}