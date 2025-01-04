#include "product/pmanager.hpp"

using M = ManagerItem;
using Pb = ProductBase;

/* Definiciones constantes para base de producto  */
const M::RecordUmap Pb::field_to_string{
    {static_cast<M::RecordField>(Pb::Rfn::id), std::string("id")},
    {static_cast<M::RecordField>(Pb::Rfn::name), std::string("name")},
    {static_cast<M::RecordField>(Pb::Rfn::description), std::string("description")},
    {static_cast<M::RecordField>(Pb::Rfn::serial), std::string("serial")},
    {static_cast<M::RecordField>(Pb::Rfn::owner), std::string("owner")},
    {static_cast<M::RecordField>(Pb::Rfn::price), std::string("price")}};

const M::ReRecordUmap Pb::string_to_field{
    {std::string("id"), static_cast<M::RecordField>(Pb::Rfn::id)},
    {std::string("name"), static_cast<M::RecordField>(Pb::Rfn::name)},
    {std::string("description"), static_cast<M::RecordField>(Pb::Rfn::description)},
    {std::string("serial"), static_cast<M::RecordField>(Pb::Rfn::serial)},
    {std::string("owner"), static_cast<M::RecordField>(Pb::Rfn::owner)},
    {std::string("price"), static_cast<M::RecordField>(Pb::Rfn::price)}};

/* Definiciones constantes para productos con nombre de vendedor */
const M::RecordUmap Product<true>::field_to_string{
    {static_cast<M::RecordField>(Product<true>::Rfn::count), std::string("count")},
    {static_cast<M::RecordField>(Product<true>::Rfn::vendor_name), std::string("vendor_name")}};

const M::ReRecordUmap Product<true>::string_to_field{
    {std::string("count"), static_cast<M::RecordField>(Product<true>::Rfn::count)},
    {std::string("vendor_name"), static_cast<M::RecordField>(Product<true>::Rfn::vendor_name)}};

/* Definiciones constantes para productos con id de vendedor  */
const M::RecordUmap Product<false>::field_to_string{
    {static_cast<M::RecordField>(Product<false>::Rfn::vendor_id), std::string("vendor_id")},
    {static_cast<M::RecordField>(Product<false>::Rfn::count), std::string("count")}};

const M::ReRecordUmap Product<false>::string_to_field{
    {std::string("vendor_id"), static_cast<M::RecordField>(Product<false>::Rfn::vendor_id)},
    {std::string("count"), static_cast<M::RecordField>(Product<false>::Rfn::count)}};
