#include "Precompile.h"
#include "MRUData.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( MRUData );

void MRUData::EnumerateClass( Reflect::Compositor<MRUData>& comp )
{
  Reflect::Field* fieldPaths = comp.AddField( &MRUData::m_Paths, "m_Paths" );
}

void MRUData::InitializeType()
{
  Reflect::RegisterClass<MRUData>( "MRUData" );
}

void MRUData::CleanupType()
{
  Reflect::UnregisterClass<MRUData>();
}

MRUData::MRUData()
{
}

MRUData::~MRUData()
{
}

const V_string& MRUData::GetPaths() const
{
  return m_Paths;
}

void MRUData::SetPaths( const V_string& paths )
{
  if ( paths != m_Paths )
  {
    m_Paths = paths;
    RaiseChanged( GetClass()->FindField( &MRUData::m_Paths ) );
  }
}