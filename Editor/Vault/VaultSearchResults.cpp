#include "Precompile.h"

#include "VaultSearchResults.h"

#include "Pipeline/Asset/AssetClass.h"

using namespace Helium;
using namespace Helium::Editor;

bool Helium::Editor::operator<( const TrackedFile& lhs, const TrackedFile& rhs )
{
    return lhs.mPath.value() < rhs.mPath.value();
}

///////////////////////////////////////////////////////////////////////////////
// Ctor/Dtor
VaultSearchResults::VaultSearchResults( uint32_t vaultSearchID )
: m_VaultSearchID( vaultSearchID )
{
}

VaultSearchResults::VaultSearchResults( const VaultSearchResults* results )
{
    HELIUM_ASSERT( results );
    m_VaultSearchID = results->m_VaultSearchID;
    m_Results = results->m_Results;
}

VaultSearchResults::~VaultSearchResults()
{
    Clear();
}

void VaultSearchResults::Clear()
{
    m_Results.clear();
}

bool VaultSearchResults::HasResults() const
{
    return !m_Results.empty();
}

void VaultSearchResults::SetResults( const std::set< TrackedFile >& results )
{
    m_Results = results;
}

const std::set< TrackedFile >& VaultSearchResults::GetResults() const
{
    return m_Results;
}

bool VaultSearchResults::Add( const TrackedFile& file )
{
    Helium::StdInsert< std::set< TrackedFile > >::Result inserted = m_Results.insert( file );
    return inserted.second;
}

bool VaultSearchResults::Remove( const TrackedFile& file )
{
    return m_Results.erase( file ) > 0;
}
