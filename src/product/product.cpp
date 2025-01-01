#include "product/pmanager.hpp"

/* Definiciones constantes para base de producto  */
const ManagerItem::RecordUmap ProductBase::field_to_string = {
    {ProductBase::Rfn::id, std::string("id")},
    {ProductBase::Rfn::name, std::string("name")},
    {ProductBase::Rfn::description, std::string("description")},
    {ProductBase::Rfn::serial, std::string("serial")},
    {ProductBase::Rfn::owner, std::string("owner")},
    {ProductBase::Rfn::price, std::string("price")}};

const ManagerItem::ReRecordUmap ProductBase::string_to_field = {
    {std::string("id"), ProductBase::Rfn::id},
    {std::string("name"), ProductBase::Rfn::name},
    {std::string("description"), ProductBase::Rfn::description},
    {std::string("serial"), ProductBase::Rfn::serial},
    {std::string("owner"), ProductBase::Rfn::owner},
    {std::string("price"), ProductBase::Rfn::price}};

/* Definiciones constantes para productos con nombre de vendedor */
const ManagerItem::RecordUmap Product<true>::field_to_string = {
    {Product<true>::Rfn::count, std::string("count")},
    {Product<true>::Rfn::vendor_name, std::string("vendor_name")}};

const ManagerItem::ReRecordUmap Product<true>::string_to_field = {
    {std::string("count"), Product<true>::Rfn::count},
    {std::string("vendor_name"), Product<true>::Rfn::vendor_name}};

/* Definiciones constantes para productos con id de vendedor  */
const ManagerItem::RecordUmap Product<false>::field_to_string = {
    {Product<false>::Rfn::vendor_id, std::string("vendor_id")},
    {Product<false>::Rfn::count, std::string("count")}};

const ManagerItem::ReRecordUmap Product<false>::string_to_field = {
    {std::string("vendor_id"), Product<false>::Rfn::vendor_id},
    {std::string("count"), Product<false>::Rfn::count}};
