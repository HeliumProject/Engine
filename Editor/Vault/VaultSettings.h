#pragma once

#include "VaultMenuIDs.h"
#include "VaultSearchQuery.h"

#include "Pipeline/Settings.h"

#include "Foundation/Container/OrderedSet.h"
#include "Editor/Settings/WindowSettings.h"

namespace Helium
{
    namespace Editor
    {
        class VaultSettings : public Settings
        {
        public:
            REFLECT_DECLARE_OBJECT( VaultSettings, Reflect::Object );

            VaultSettings( VaultViewMode viewVaultMode = VaultViewMode::Details, uint32_t thumbnailSize = VaultThumbnailsSizes::Medium );
            ~VaultSettings();

        public:
            VaultViewMode         m_VaultViewMode;
            uint32_t                   m_ThumbnailSize;

        public:
            static void AcceptCompositeVisitor( Reflect::Composite& comp )
            {
                comp.AddEnumerationField( &VaultSettings::m_VaultViewMode, TXT( "Vault View Mode" ) );

                {
                    Reflect::Field* field = comp.AddField( &VaultSettings::m_ThumbnailSize, TXT( "Thumbnail Size" ) );
                    field->SetProperty( TXT( "UIScript" ), TXT( "UI[.[slider{min=16.0; max=256.0} value{}].]" ) );
                }
            }
        };

        typedef Helium::SmartPtr< VaultSettings > VaultSettingsPtr;
    }
}