#include "EditorPch.h"
#include "VaultSettings.h"
#include "Editor/Vault/VaultPanel.h"

using namespace Helium;
using namespace Helium::Editor;

REFLECT_DEFINE_OBJECT( VaultSettings );

void VaultSettings::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddEnumerationField( &VaultSettings::m_VaultViewMode, TXT( "Vault View Mode" ) );

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
