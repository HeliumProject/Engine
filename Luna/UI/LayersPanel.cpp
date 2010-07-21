#include "Precompile.h"

#include "LayersPanel.h"
#include "ArtProvider.h"

using namespace Luna;

LayersPanel::LayersPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: LayersPanelGenerated( parent, id, pos, size, style )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_LayersToolbar->FindById( ID_CreateNewLayer )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::CreateNewLayer ) );
        m_LayersToolbar->FindById( ID_CreateNewLayerFromSelection )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::CreateNewLayerFromSelection ) );
        m_LayersToolbar->FindById( ID_DeleteSelectedLayers )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::DeleteSelectedLayers ) );
        m_LayersToolbar->FindById( ID_AddSelectionToLayers )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::AddSelectionToLayers ) );
        m_LayersToolbar->FindById( ID_RemoveSelectionFromLayers )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::RemoveSelectionFromLayers ) );
        m_LayersToolbar->FindById( ID_SelectLayerMembers )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::SelectLayerMembers ) );
        m_LayersToolbar->FindById( ID_SelectLayers )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::SelectLayers ) );

        m_LayersToolbar->Realize();

        Layout();
        Thaw();
    }

}
