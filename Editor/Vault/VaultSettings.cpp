#include "EditorPch.h"
#include "VaultSettings.h"
#include "Editor/Vault/VaultPanel.h"

REFLECT_DEFINE_OBJECT( Helium::Editor::VaultSettings );

using namespace Helium;
using namespace Helium::Editor;

void VaultSettings::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &VaultSettings::m_VaultViewMode, TXT( "Vault View Mode" ) );

    Reflect::Field* field = NULL;

    field = comp.AddField( &VaultSettings::m_ThumbnailSize, TXT( "Thumbnail Size" ) );
    field->SetProperty( TXT( "UIScript" ), TXT( "UI[.[slider{min=16.0; max=256.0} value{}].]" ) );
}

VaultSettings::VaultSettings( VaultViewMode viewVaultMode, uint32_t thumbnailSize )
: m_VaultViewMode( viewVaultMode )
, m_ThumbnailSize( thumbnailSize )
{
}

VaultSettings::~VaultSettings()
{
}
