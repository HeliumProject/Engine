#include "EditorPch.h"
#include "VaultSearchResults.h"

using namespace Helium;
using namespace Helium::Editor;

bool Helium::Editor::operator<( const TrackedFile& lhs, const TrackedFile& rhs )
{
#pragma TODO("Implement this")
    return true;
}

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
    std::pair<  std::set< TrackedFile > ::const_iterator, bool > inserted = m_Results.insert( file );
    return inserted.second;
}

bool VaultSearchResults::Remove( const TrackedFile& file )
{
    return m_Results.erase( file ) > 0;
}
