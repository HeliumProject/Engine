#include "Precompile.h"
#include "PreviewPanel.h"

#include "BrowserFrame.h"
#include "BrowserPreferences.h"

#include "Asset/AssetClass.h"
#include "Asset/ArtFileAttribute.h"
#include "Attribute/AttributeHandle.h"
#include "UIToolKit/ImageManager.h"
#include "UIToolKit/MenuButton.h"

using namespace Luna;

PreviewPanel::PreviewPanel( BrowserFrame* browserFrame )
: PreviewPanelGenerated( browserFrame, wxID_ANY )
, m_BrowserFrame( browserFrame )
{
    m_PreviewWindow->SetBrowserFrame( m_BrowserFrame );

    m_Label->SetValue( "" );
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

        m_Label->SetValue( "" );

        if ( m_PreviewAsset.ReferencesObject() )
        {
            m_Label->SetValue( m_PreviewAsset->GetFilePath().Get() );

            Attribute::AttributeViewer< Asset::ArtFileAttribute > artFile( m_PreviewAsset );
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

