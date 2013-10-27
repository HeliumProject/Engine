#pragma once

#include "Application/OrderedSet.h"

#include "EditorScene/SettingsManager.h"

#include "Editor/Vault/VaultMenuIDs.h"
#include "Editor/Vault/VaultSearchQuery.h"
#include "Editor/Settings/WindowSettings.h"

namespace Helium
{
    namespace Editor
    {
        class VaultSettings : public Settings
        {
        public:
            HELIUM_DECLARE_CLASS( VaultSettings, Reflect::Object );
            static void PopulateMetaType( Reflect::MetaStruct& comp );

            VaultSettings( VaultViewMode viewVaultMode = VaultViewMode::Details, uint32_t thumbnailSize = VaultThumbnailsSizes::Medium );
            ~VaultSettings();

        public:
            VaultViewMode         m_VaultViewMode;
            uint32_t                   m_ThumbnailSize;
        };

        typedef Helium::SmartPtr< VaultSettings > VaultSettingsPtr;
    }
}