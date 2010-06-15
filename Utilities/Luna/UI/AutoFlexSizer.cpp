#include "Precompile.h"
#include "AutoFlexSizer.h"

using namespace Luna;

AutoFlexSizer::AutoFlexSizer( int vgap, int hgap )
: wxFlexGridSizer( 0, 0, vgap, hgap )
{
}

AutoFlexSizer::~AutoFlexSizer()
{
}

void AutoFlexSizer::RecalcSizes()
{
    UpdateRowsCols();

    __super::RecalcSizes();
}

wxSize AutoFlexSizer::CalcMin()
{
    UpdateRowsCols();

    return __super::CalcMin();
}

void AutoFlexSizer::UpdateRowsCols()
{
    int width = m_containingWindow->GetSize().x;

    int numColumns = 1;
    int maxColumns = (int) m_children.GetCount();
    for ( int i = 1; i <= maxColumns; ++i )
    {
        if ( GetMaxColumnWidth( i ) > width )
        {
            break;
        }

        numColumns = i;
    }

    m_rows = 0;
    if ( m_cols != numColumns )
    {
        m_cols = numColumns;
    }
}

int AutoFlexSizer::GetMaxColumnWidth( int numItems )
{
    int maxWidth = 0;

    int currentWidth = 0;
    int currentIndex = 0;

    wxSizerItemList::compatibility_iterator node = m_children.GetFirst();
    while ( node )
    {
        if ( currentIndex >= numItems )
        {
            maxWidth = wxMax( maxWidth, currentWidth );
            currentIndex = 0;
            currentWidth = 0;
        }

        wxSizerItem *item = node->GetData();
        wxSize itemSize( item->CalcMin() );

        if ( currentWidth )
        {
            currentWidth += m_hgap;
        }

        currentWidth += itemSize.x;
        ++currentIndex;

        node = node->GetNext();
    }

    return wxMax( maxWidth, currentWidth );
}
