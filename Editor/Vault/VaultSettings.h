#pragma once

#include "VaultMenuIDs.h"
#include "VaultSearchQuery.h"

#include "Foundation/TUID.h"
#include "Foundation/Container/OrderedSet.h"
#include "Editor/WindowSettings.h"

namespace Helium
{
    namespace Editor
    {
        class VaultSettings : public Reflect::Element
        {
        public:
            REFLECT_DECLARE_CLASS( VaultSettings, Reflect::Element );

            VaultSettings( VaultViewMode viewVaultMode = VaultViewMode::Details, uint32_t thumbnailSize = VaultThumbnailsSizes::Medium );
            ~VaultSettings();

        public:
            VaultViewMode         m_VaultViewMode;
            uint32_t                   m_ThumbnailSize;
            //WindowSettingsPtr     m_WindowSettings;

        public:
            static void AcceptCompositeVisitor( Reflect::Composite& comp )
            {
                comp.AddEnumerationField( &VaultSettings::m_VaultViewMode, "m_VaultViewMode" );

                {
                    Reflect::Field* field = comp.AddField( &VaultSettings::m_ThumbnailSize, "m_ThumbnailSize" );
                    field->SetProperty( TXT( "UIScript" ), TXT( "UI[.[slider{min=16.0; max=256.0} value{}].]" ) );
                }
                //comp.AddField( &VaultSettings::m_WindowSettings, "m_WindowSettings", Reflect::FieldFlags::Hide );
            }
        };

        typedef Helium::SmartPtr< VaultSettings > VaultSettingsPtr;
    }
}