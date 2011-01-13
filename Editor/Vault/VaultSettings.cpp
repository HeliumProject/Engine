#include "Precompile.h"
#include "VaultSettings.h"
#include "Editor/Vault/VaultPanel.h"

using namespace Helium;
using namespace Helium::Editor;

REFLECT_DEFINE_OBJECT( VaultSettings );

///////////////////////////////////////////////////////////////////////////////
VaultSettings::VaultSettings( VaultViewMode viewVaultMode, uint32_t thumbnailSize )
: m_VaultViewMode( viewVaultMode )
, m_ThumbnailSize( thumbnailSize )
{
}

VaultSettings::~VaultSettings()
{
}
