#include "Precompile.h"
#include "TreeNodeWidget.h"

#include "Editor/FileIconsTable.h"

using namespace Helium;
using namespace Helium::Editor;

TreeNodeWidget::TreeNodeWidget( Inspect::Container* container )
: m_ContainerControl( container )
, m_TreeWndCtrl( NULL )
, m_ItemData( this )
{
    SetControl( container );
}

void TreeNodeWidget::Create( wxWindow* parent )
{
    // find the parent TreeNodeWidget
    Inspect::Container*     parentContainer = m_ContainerControl->GetParent();
    TreeNodeWidget*         parentWidget    = Reflect::AssertCast< TreeNodeWidget >( parentContainer->GetWidget() );
    wxTreeItemId            parentId        = parentWidget->m_ItemData.GetId();

    m_TreeWndCtrl = parentWidget->m_TreeWndCtrl;
    m_TreeWndCtrl->Freeze();

    bool childrenAreContainers = true;
    {
        Inspect::V_Control::const_iterator itr = m_ContainerControl->GetChildren().begin();
        Inspect::V_Control::const_iterator end = m_ContainerControl->GetChildren().end();
        for ( ; itr != end; ++itr )
        {
            if ( !(*itr)->HasType( Reflect::GetType< Inspect::Container >() ) )
            {
                childrenAreContainers = false;
                break;
            }
        }
    }

    if ( childrenAreContainers )
    {
        // our window is the treewndctrl
        m_Window = m_TreeWndCtrl;

        // add a tree item for this container
        int collapsedIndex = GlobalFileIconsTable().GetIconID( TXT( "ms_folder_closed" ) );
        int expandedIndex = GlobalFileIconsTable().GetIconID( TXT( "ms_folder_open" ) );
        m_TreeWndCtrl->AppendItem( parentId, m_ContainerControl->a_Name.Get(), collapsedIndex, expandedIndex, &m_ItemData );

        // realize child controls
        for( Inspect::V_Control::const_iterator itr = m_ContainerControl->GetChildren().begin(), end = m_ContainerControl->GetChildren().end(); itr != end; ++itr )
        {
            Inspect::Control* c = *itr;
            c->Realize( m_ContainerControl->GetCanvas() );
        }

        m_TreeWndCtrl->Layout();
    }
    else // children are controls
    {
        // our window is a panel of child controls
        m_Window = new wxPanel( m_TreeWndCtrl, wxID_ANY );
        m_Window->SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
        wxSizer* sizer = m_Window->GetSizer();

        std::vector< wxSizer* > sizerList;
        std::vector< i32 > proportionList;

        int proportionMultiplier = 1000;
        int remainingProportion = proportionMultiplier;
        int numRemainingProportions = 0;

        wxSizer* unboundedProportionSizer = NULL;

        for ( Inspect::V_Control::const_iterator itr = m_ContainerControl->GetChildren().begin(), end = m_ContainerControl->GetChildren().end(); itr != end; ++itr )
        {
            Inspect::Control* c = *itr;
            c->Realize( m_ContainerControl->GetCanvas() );

            if ( c->a_ProportionalWidth.Get() > 0.0f )
            {
                int proportion = (int) ( c->a_ProportionalWidth.Get() * (f32) proportionMultiplier );
                remainingProportion -= proportion;

                sizerList.push_back( new wxBoxSizer( wxHORIZONTAL ) );
                proportionList.push_back( proportion );
                unboundedProportionSizer = NULL;
            }
            else
            {
                if ( unboundedProportionSizer == NULL )
                {
                    unboundedProportionSizer = new wxBoxSizer( wxHORIZONTAL );
                    ++numRemainingProportions;
                }

                sizerList.push_back( unboundedProportionSizer );
                proportionList.push_back( -1 );
            }
        }

        if ( numRemainingProportions > 1 )
        {
            remainingProportion = (int) ( (f32) remainingProportion / (f32) numRemainingProportions + 0.5f );
        }

        int spacing = m_ContainerControl->GetCanvas()->GetPad();

        int index = 0;
        Inspect::V_Control::const_iterator itr = m_ContainerControl->GetChildren().begin();
        Inspect::V_Control::const_iterator end = m_ContainerControl->GetChildren().end();
        for( ; itr != end; ++index, ++itr )
        {
            Inspect::Control* c = *itr;

            int proportion = proportionList[ index ];
            wxSizer* currentSizer = sizerList[ index ];

            if ( sizer->GetItem( currentSizer ) == NULL )
            {
                sizer->Add( currentSizer, proportion > 0 ? proportion : remainingProportion, wxEXPAND | wxTOP | wxBOTTOM, spacing );
            }

            int flags = wxALIGN_CENTER_VERTICAL;
            proportion = 0;
            if ( !c->a_IsFixedWidth.Get() )
            {
                proportion = 1;
                flags |= wxEXPAND;
            }

            currentSizer->Add( spacing, 0, 0 );
            currentSizer->Add( Reflect::AssertCast< Widget >( c->GetWidget() )->GetWindow(), proportion, flags );
        }
        sizer->Add(spacing, 0, 0);

        m_Window->Layout();

        // add the panel to the tree
        m_TreeWndCtrl->AppendItem( parentId, m_Window );
    }

    m_TreeWndCtrl->Thaw();
}

void TreeNodeWidget::Destroy()
{
    HELIUM_ASSERT( false );
}