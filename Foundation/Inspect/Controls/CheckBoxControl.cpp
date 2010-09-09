#include "Foundation/Inspect/Controls/CheckBoxControl.h"
#include "Foundation/Inspect/Container.h"

using namespace Helium::Inspect;

CheckBox::CheckBox()
: a_Highlight( false )
{
    m_ContextMenu = new ContextMenu( this );
    m_ContextMenu->AddItem( TXT( "Set To Default" ), ContextMenuSignature::Delegate( this, &CheckBox::SetToDefault ) );
}

void CheckBox::SetDefaultAppearance( bool def ) 
{
    a_Highlight.Set( def );
}

void CheckBox::SetToDefault( const ContextMenuEventArgs& event )
{
    event.m_Control->SetDefault();
    event.m_Control->Read();
}
