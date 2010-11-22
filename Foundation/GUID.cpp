#include "GUID.h"
#include "TUID.h"

#include "Platform/Types.h"

#define RPC_USE_NATIVE_WCHAR
#include <objbase.h>

#pragma comment( lib, "rpcrt4.lib" )

#ifdef UNICODE
# define RPC_TSTR RPC_WSTR
#else
# define RPC_TSTR RPC_CSTR
#endif

using namespace Helium;

const Helium::GUID Helium::GUID::Null;

Helium::GUID::GUID()
{
    Reset();
}

Helium::GUID::GUID(const GUID &id)
{
    (*this)=id;
}

Helium::GUID& Helium::GUID::operator=(const Helium::GUID &rhs)
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

bool Helium::GUID::operator==(const Helium::GUID &rhs) const
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

bool Helium::GUID::operator!=(const Helium::GUID &rhs) const
{
    return !((*this)==rhs);
}

bool Helium::GUID::operator<(const Helium::GUID &rhs) const
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

void Helium::GUID::ToTUID( tuid& id ) const
{
    TUID t;
    t.FromGUID( *this );
    id = (tuid)t;
}

void Helium::GUID::ToString(tstring& id) const
{
    tchar_t* l_pszString;

    UuidToString((UUID*)(this), (RPC_TSTR*)&l_pszString);

    id = reinterpret_cast<tchar_t *>(l_pszString);

    RpcStringFree((RPC_TSTR*)&l_pszString);
}

void Helium::GUID::FromTUID( tuid id )
{
    TUID t( id );
    t.ToGUID( *this );
}

bool Helium::GUID::FromString(const tstring& id)
{
    GUID uid;

    if (RPC_S_OK == UuidFromString((RPC_TSTR)id.data(), reinterpret_cast<UUID*>(&uid)))
    {
        (*this)=uid;
        return true;
    }

    return false;
}

void Helium::GUID::Reset()
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

Helium::GUID Helium::GUID::Generate()
{
    GUID uid;
    UuidCreate(reinterpret_cast<UUID*>(&uid));
    return uid;
}

void Helium::GUID::Generate (GUID& uid)
{
    UuidCreate(reinterpret_cast<UUID*>(&uid));
}