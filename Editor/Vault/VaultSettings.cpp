#include "EditorPch.h"
#include "VaultSettings.h"
#include "Editor/Vault/VaultPanel.h"

HELIUM_DEFINE_CLASS( Helium::Editor::VaultSettings );

using namespace Helium;
using namespace Helium::Editor;

void VaultSettings::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &VaultSettings::m_VaultViewMode, "Vault View Mode" );

    Reflect::Field* field = NULL;

    field = comp.AddField( &VaultSettings::m_ThumbnailSize, "Thumbnail Size" );
    field->SetProperty( "UIScript", "UI[.[slider{min=16.0; max=256.0} value{}].]" );
}

VaultSettings::VaultSettings( VaultViewMode viewVaultMode, uint32_t thumbnailSize )
: m_VaultViewMode( viewVaultMode )
, m_ThumbnailSize( thumbnailSize )
{
}

VaultSettings::~VaultSettings()
{
}
