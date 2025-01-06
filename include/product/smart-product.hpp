#include "forwarder.hpp"
#include "product/product.hpp"
#include <unistd.h>     // Basic system calls
#include <sys/socket.h> // Socket system calls
#include <netinet/in.h> // Internet address family and structures
#include <arpa/inet.h>  // Functions for manipulating numeric IP addresses

class SmartProductBase : public ProductBase
{
public:
  // Record binding for extractRecord calls
  static const RecordUmap field_to_string;
  static const ReRecordUmap string_to_field;

  // Stores all the ports it uses to check if the remote machine is active
  static const vec<std::uint16_t> liveness_check_ports;

  // Miembros publicos no estaticos
  std::string inaddr;

  SmartProductBase()
      : ProductBase(), inaddr()
  {
  }

  SmartProductBase(const RecordUmap record, const bool vrtl)
      : ProductBase(record, vrtl),
        inaddr(field_to_string.at(P_Inaddr))
  {
  }

  SmartProductBase(const SmartProductBase &other)
      : ProductBase(other), inaddr(other.inaddr)
  {
  }

  SmartProductBase(SmartProductBase &&other)
      : ProductBase(std::move(other)), inaddr(std::move(other.inaddr))
  {
  }

  // Works over liveness_check_ports to check if the remote machine is active
  bool checkLiveness();

  // Extracts the record binding
  virtual std::pair<RecordUmap, ReRecordUmap> extractRecordBinding() const noexcept override
  {
    return std::make_pair(field_to_string, string_to_field);
  }

  virtual RecordUmap extractRecord() const noexcept override
  {
    RecordUmap record = ProductBase::extractRecord();
    record.emplace(P_Inaddr, inaddr);

    return record;
  };

  // Operadores

  virtual SmartProductBase &operator=(const SmartProductBase &other)
  {
    if (this != &other)
    {
      inaddr = other.inaddr;
    }
    return *this;
  }

  virtual SmartProductBase &operator=(SmartProductBase &&other)
  {
    if (this != &other)
    {
      inaddr = std::move(other.inaddr);
    }
    return *this;
  }

  virtual ~SmartProductBase() = default;
};

class SmartProduct : public SmartProductBase
{
public:
  // Record binding for extractRecord calls
  static const RecordUmap field_to_string;
  static const ReRecordUmap string_to_field;

  bool is_active;

  SmartProduct()
      : SmartProductBase(), is_active(false)
  {
  }

  SmartProduct(const RecordUmap record, const bool vrtl)
      : SmartProductBase(record, vrtl),
        is_active(std::stoul(record.at(P_IsActive)))
  {
  }

  SmartProduct(const SmartProduct &other)
      : SmartProductBase(other), is_active(other.is_active)
  {
  }

  SmartProduct(SmartProduct &&other)
      : SmartProductBase(std::move(other)), is_active(other.is_active)
  {
    other.is_active = false;
  }

  // Extracts the record binding
  std::pair<RecordUmap, ReRecordUmap> extractRecordBinding() const noexcept override
  {
    return std::make_pair(field_to_string, string_to_field);
  }

  RecordUmap extractRecord() const noexcept override
  {
    RecordUmap record = SmartProductBase::extractRecord();
    record.emplace(P_Inaddr, inaddr);

    return record;
  };

  // Operators

  SmartProduct &operator=(const SmartProduct &other)
  {
    if (this != &other)
    {
      is_active = other.is_active;
    }
    return *this;
  }

  SmartProduct &operator=(SmartProduct &&other)
  {
    if (this != &other)
    {
      is_active = other.is_active;
      other.is_active = false;
    }
    return *this;
  }

  ~SmartProduct()
  {
    is_active = false;
  }
};