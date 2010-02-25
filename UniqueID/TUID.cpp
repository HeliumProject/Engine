#include "TUID.h"

#include "TUID/TUID.h"
#include <sstream>

const UniqueID::TUID UniqueID::TUID::Null = 0x0;

void UniqueID::TUID::ToGUID(UniqueID::GUID& id) const
{
  ::TUID::Upsample(*this, (guid&)id);
}

void UniqueID::TUID::ToString(std::string& id) const
{
  std::ostringstream str;
  str << ::TUID::HexFormat << m_ID;
  id = str.str();
}

void UniqueID::TUID::FromGUID(const UniqueID::GUID& id)
{
  ::TUID::Downsample((const guid&)id, m_ID);
}

bool UniqueID::TUID::FromString(const std::string& id)
{
  return ::TUID::Parse(id, m_ID);
}

UniqueID::TUID UniqueID::TUID::Generate()
{
  return ::TUID::Generate();
}

void UniqueID::TUID::Generate (UniqueID::TUID& uid)
{
  uid = ::TUID::Generate();
}