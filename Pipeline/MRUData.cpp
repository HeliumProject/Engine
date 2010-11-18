/*#include "Precompile.h"*/
#include "MRUData.h"

#include "Platform/Debug.h"

using namespace Helium;

REFLECT_DEFINE_CLASS( MRUData );

const std::vector< Helium::Path >& MRUData::GetPaths() const
{
    return m_Paths;
}

void MRUData::SetPaths( const std::vector< Helium::Path >& paths )
{
    if ( paths != m_Paths )
    {
        m_Paths = paths;
        RaiseChanged( GetClass()->FindField( &MRUData::m_Paths ) );
    }
}
