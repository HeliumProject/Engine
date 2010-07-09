#include "Precompile.h"
#include "PreviewPanel.h"

#include "VaultFrame.h"
#include "VaultPreferences.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Classes/Entity.h"
#include "Foundation/Component/ComponentHandle.h"
#include "Application/UI/ArtProvider.h"
#include "Application/UI/MenuButton.h"

using namespace Luna;

PreviewPanel::PreviewPanel( VaultFrame* browserFrame )
: PreviewPanelGenerated( browserFrame, wxID_ANY )
, m_VaultFrame( browserFrame )
{
    m_PreviewWindow->SetVaultFrame( m_VaultFrame );

    m_Label->SetValue( TXT( "" ) );
}

PreviewPanel::~PreviewPanel()
{
}

void PreviewPanel::Preview( Asset::AssetClass* asset )
{
    if ( m_PreviewAsset.Ptr() != asset )
    {
        wxBusyCursor busyCursor;
        m_ContentFile.clear();
        m_PreviewAsset = asset;
        m_PreviewWindow->ClearScene();

        m_Label->SetValue( TXT( "" ) );

        if ( m_PreviewAsset.ReferencesObject() )
        {
            m_Label->SetValue( m_PreviewAsset->GetPath().Get() );

            Asset::EntityPtr entity = Reflect::ObjectCast< Asset::Entity >( asset );
            if ( entity.ReferencesObject() )
            {
                m_ContentFile = entity->GetPath().Get();
                }
            }

        if ( !m_ContentFile.empty() )
        {
            if ( !m_PreviewWindow->LoadScene( m_ContentFile ) )
            {
                // Do something?
            }
        }
    }
}

void PreviewPanel::DisplayReferenceAxis( bool display )
{
    m_PreviewWindow->DisplayReferenceAxis( display );
}

