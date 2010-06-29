#include "Precompile.h"
#include "PreviewPanel.h"

#include "BrowserFrame.h"
#include "BrowserPreferences.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Components/ArtFileComponent.h"
#include "Pipeline/Component/ComponentHandle.h"
#include "Application/UI/ImageManager.h"
#include "Application/UI/MenuButton.h"

using namespace Luna;

PreviewPanel::PreviewPanel( BrowserFrame* browserFrame )
: PreviewPanelGenerated( browserFrame, wxID_ANY )
, m_BrowserFrame( browserFrame )
{
    m_PreviewWindow->SetBrowserFrame( m_BrowserFrame );

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
            m_Label->SetValue( m_PreviewAsset->GetFilePath().Get() );

            Component::ComponentViewer< Asset::ArtFileComponent > artFile( m_PreviewAsset );
            if ( artFile.Valid() )
            {
                if ( !artFile->GetPath().Get().empty() )
                {
                    m_ContentFile = artFile->GetPath().Get();
                }
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

