#include "product/pmanager.hpp"

using M = ManagerItem;
using Pb = ProductBase;

/* Definiciones constantes para base de producto  */
const M::RecordUmap Pb::field_to_string{
    {P_Id, std::string("id")},
    {P_Name, std::string("name")},
    {P_Description, std::string("description")},
    {P_Serial, std::string("serial")},
    {P_Owner, std::string("owner")},
    {P_Price, std::string("price")}};

const M::ReRecordUmap Pb::string_to_field{
    {std::string("id"), P_Id},
    {std::string("name"), P_Name},
    {std::string("description"), P_Description},
    {std::string("serial"), P_Serial},
    {std::string("owner"), P_Owner},
    {std::string("price"), P_Price}};

/* Definiciones constantes para productos con nombre de vendedor */
const M::RecordUmap Product<true>::field_to_string =
    Pb::forkRecordBinding({{P_Count,
                            std::string("count")},
                           {P_VendorName,
                            std::string("vendor_name")}});

const M::ReRecordUmap Product<true>::string_to_field =
    Pb::forkRecordBinding({{std::string("count"), P_Count},
                           {std::string("vendor_name"), P_VendorName}});

/* Definiciones constantes para productos con id de vendedor  */
const M::RecordUmap Product<false>::field_to_string =
    Pb::forkRecordBinding({{P_VendorId, std::string("vendor_id")},
                           {P_Count, std::string("count")}});

const M::ReRecordUmap Product<false>::string_to_field =
    Pb::forkRecordBinding({{std::string("vendor_id"), P_VendorId},
                           {std::string("count"), P_Count}});