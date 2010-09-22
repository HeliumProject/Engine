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
        class VaultSettings : public Reflect::ConcreteInheritor< VaultSettings, Reflect::Element >
        {
        public:
            VaultSettings( VaultViewMode viewVaultMode = VaultViewModes::Details,
                u32 thumbnailSize = VaultThumbnailsSizes::Medium );
            ~VaultSettings();

        public:
            VaultViewMode         m_VaultViewMode;
            u32                   m_ThumbnailSize;
            //WindowSettingsPtr     m_WindowSettings;

        public:
            static void EnumerateClass( Reflect::Compositor< VaultSettings >& comp )
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