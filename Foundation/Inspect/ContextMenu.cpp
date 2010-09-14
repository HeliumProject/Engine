#include "Foundation/Inspect/ContextMenu.h"
#include "Foundation/Inspect/Container.h"

#include <memory>

using namespace Helium::Inspect;

ContextMenu::ContextMenu(Control* control)
: m_Control (control)
{

}

ContextMenu::~ContextMenu()
{

}

void ContextMenu::AddItem(const tstring& item, ContextMenuSignature::Delegate delegate)
{
    M_ContextMenuDelegate::iterator found = m_Delegates.find(item);

    if (found == m_Delegates.end())
    {
        m_Items.push_back(item);
        m_Delegates.insert(M_ContextMenuDelegate::value_type(item, delegate));
    }
    else
    {
        found->second = delegate;
    }
}

void ContextMenu::AddSeperator()
{
    m_Items.push_back( TXT( "-" ) );
}