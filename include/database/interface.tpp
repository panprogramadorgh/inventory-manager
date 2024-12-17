template <typename T>
QueryUmap<T> Database::umapQuery(QueryResult qresult)
  requires std::is_same_v<ProductInfo, T>
{
  QueryUmap<Product> dest;
  UmappedProduct row;
  ProductField field;

  auto cols = qresult.first;
  auto vals = qresult.second;

  for (auto it = vals.begin();
       it != vals.cend() &&
       std::distance(vals.begin(), it) % cols.size() < cols.size();
       it++)
  {
    field = string_to_product_field.at(cols[std::distance(vals.begin(), it) % cols.size()]);
    row[field] = *it;

    // En caso de que sea la ulima columna por registro
    if (std::distance(vals.begin(), it) % cols.size() == cols.size() - 1)
    {
      // Creates a non-virtual ProductInfo
      ProductInfo pinfo(row, false);
      dest.emplace(pinfo.product_id, std::make_shared<Product>(Product(pinfo)));
      row.clear(); // Limpiar el mapa para la proxima fila
    }
  }

  return dest;
}

template <typename T>
static QueryUmap<T> umapQuery(QueryResult qresult)
  requires std::is_same_v<Product, T>
{
  return QueryUmap<Product>();
}

template <typename T>
static void printQuery(const QueryUmap<T> qumap) noexcept
  requires std::is_same_v<ProductInfo, T>
{
  ProductInfo pinfo; // Intermediate step to umap product
  UmappedProduct up;

  for (auto fit = product_field_to_string.cbegin(); fit != product_field_to_string.cend(); fit++)
  {
    if (fit != product_field_to_string.cbegin())
      std::cout << ',';
    std::cout << fit->second;
  }
  for (auto pit = qumap.cbegin(); pit != qumap.cend(); pit++)
  {
    pinfo = pit->second->info();
    up = Product::umapProduct(pinfo);
    for (auto fit = up.cbegin(); fit != up.cend(); fit++)
    {
      if (fit != up.cbegin())
        std::cout << ',';
      std::cout << fit->second;
    }
    std::cout << std::endl;
  }
}

template <typename T>
static void printQuery(const QueryUmap<T> qumap) noexcept
  requires std::is_same_v<Product, T>
{
  return;
}