#include "GUID.h"

#include "TUID/TUID.h"
#include <objbase.h>

const UniqueID::GUID UniqueID::GUID::Null;

UniqueID::GUID::GUID()
{
  Reset();
}

UniqueID::GUID::GUID(const UniqueID::GUID &id)
{
  (*this)=id;
}

UniqueID::GUID& UniqueID::GUID::operator=(const UniqueID::GUID &rhs)
{
  Data1 = rhs.Data1;
  Data2 = rhs.Data2;
  Data3 = rhs.Data3;

  Data4[0] = rhs.Data4[0];
  Data4[1] = rhs.Data4[1];
  Data4[2] = rhs.Data4[2];
  Data4[3] = rhs.Data4[3];
  Data4[4] = rhs.Data4[4];
  Data4[5] = rhs.Data4[5];
  Data4[6] = rhs.Data4[6];
  Data4[7] = rhs.Data4[7];

  return (*this);
}

bool UniqueID::GUID::operator==(const UniqueID::GUID &rhs) const
{
  if (Data1 == rhs.Data1 &&
      Data2 == rhs.Data2 &&
      Data3 == rhs.Data3 &&
      Data4[0] == rhs.Data4[0] &&
      Data4[1] == rhs.Data4[1] &&
      Data4[2] == rhs.Data4[2] &&
      Data4[3] == rhs.Data4[3] &&
      Data4[4] == rhs.Data4[4] &&
      Data4[5] == rhs.Data4[5] &&
      Data4[6] == rhs.Data4[6] &&
      Data4[7] == rhs.Data4[7])
    return true;

  return false;
}

bool UniqueID::GUID::operator!=(const UniqueID::GUID &rhs) const
{
  return !((*this)==rhs);
}

bool UniqueID::GUID::operator<(const UniqueID::GUID &rhs) const
{
  if ( Data1 != rhs.Data1 )
    return Data1 < rhs.Data1;

  else if ( Data2 != rhs.Data2 )
    return Data2 < rhs.Data2;

  else if ( Data3 != rhs.Data3 )
    return Data3 < rhs.Data3;

  else if ( Data4[0] != rhs.Data4[0] )
    return Data4[0] < rhs.Data4[0];

  else if ( Data4[1] != rhs.Data4[1] )
    return Data4[1] < rhs.Data4[1];

  else if ( Data4[2] != rhs.Data4[2] )
    return Data4[2] < rhs.Data4[2];

  else if ( Data4[3] != rhs.Data4[3] )
    return Data4[3] < rhs.Data4[3];

  else if ( Data4[4] != rhs.Data4[4] )
    return Data4[4] < rhs.Data4[4];

  else if ( Data4[5] != rhs.Data4[5] )
    return Data4[5] < rhs.Data4[5];

  else if ( Data4[6] != rhs.Data4[6] )
    return Data4[6] < rhs.Data4[6];

  else if ( Data4[7] != rhs.Data4[7] )
    return Data4[7] < rhs.Data4[7];

  return false;
}

void UniqueID::GUID::ToTUID(tuid& id) const
{
  ::TUID::Downsample((const guid&)*this, id);
}

void UniqueID::GUID::ToString(std::string& id) const
{
  unsigned char *l_pszString;

  UuidToStringA((UUID*)(this), &l_pszString);
  
  id = reinterpret_cast<char *>(l_pszString);

  RpcStringFree(&l_pszString);
}

void UniqueID::GUID::FromTUID(tuid id)
{
  ::TUID::Upsample(id, (guid&)*this);
}

bool UniqueID::GUID::FromString(const std::string& id)
{
  UniqueID::GUID uid;

  if (RPC_S_OK == UuidFromStringA((unsigned char *)id.data(), reinterpret_cast<UUID*>(&uid)))
  {
    (*this)=uid;
    return true;
  }

  return false;
}

void UniqueID::GUID::Reset()
{
  Data1 = 0;
  Data2 = 0;
  Data3 = 0;
  Data4[0] = 0;
  Data4[1] = 0;
  Data4[2] = 0;
  Data4[3] = 0;
  Data4[4] = 0;
  Data4[5] = 0;
  Data4[6] = 0;
  Data4[7] = 0;
}

UniqueID::GUID UniqueID::GUID::Generate()
{
  UniqueID::GUID uid;
  UuidCreate(reinterpret_cast<UUID*>(&uid));
  return uid;
}

void UniqueID::GUID::Generate (UniqueID::GUID& uid)
{
  UuidCreate(reinterpret_cast<UUID*>(&uid));
}