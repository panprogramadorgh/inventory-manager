#include "product/product.hpp"

using M = ManagerItem;
using Pb = ProductBase;

/* Definiciones constantes para base de producto  */
const M::RecordUmap Pb::field_to_string{
    {P_Id, "id"},
    {P_Name, "name"},
    {P_Description, "description"},
    {P_Serial, "serial"},
    {P_Owner, "owner"},
    {P_Price, "price"}};

const M::ReRecordUmap Pb::string_to_field{
    {"id", P_Id},
    {"name", P_Name},
    {"description", P_Description},
    {"serial", P_Serial},
    {"owner", P_Owner},
    {"price", P_Price}};

/* Definiciones constantes para productos con nombre de vendedor */
const M::RecordUmap Product<true>::field_to_string =
    M::forkRecordBinding(Pb(), {{P_Count, "count"},
                                {P_VendorName, "vendor_name"}});

const M::ReRecordUmap Product<true>::string_to_field =
    M::forkRecordBinding(Pb(), {{"count", P_Count},
                                {"vendor_name", P_VendorName}});

/* Definiciones constantes para productos con id de vendedor  */
const M::RecordUmap Product<false>::field_to_string =
    M::forkRecordBinding(Pb(), {{P_VendorId, "vendor_id"},
                                {P_Count, "count"}});

const M::ReRecordUmap Product<false>::string_to_field =
    M::forkRecordBinding(Pb(), {{"vendor_id", P_VendorId},
                                {"count", P_Count}});