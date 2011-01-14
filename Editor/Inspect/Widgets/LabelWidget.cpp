#include "Precompile.h"
#include "LabelWidget.h"

#include <wx/panel.h>
#include <wx/stattext.h>

using namespace Helium;
using namespace Helium::Editor;

LabelWindow::LabelWindow( wxWindow* parent, LabelWidget* labelWidget, bool ellipsize )
: wxStaticText( parent, wxID_ANY, wxT( "Temp" ), wxDefaultPosition, wxDefaultSize, ellipsize ? wxST_ELLIPSIZE_END : 0 )
, m_LabelWidget (labelWidget)
{

}

REFLECT_DEFINE_OBJECT( LabelWidget );

LabelWidget::LabelWidget( Inspect::Label* label )
: m_LabelControl( label )
, m_LabelWindow( NULL )
{
    SetControl( label );
}

void LabelWidget::CreateWindow( wxWindow* parent )
{
    HELIUM_ASSERT( !m_LabelWindow );

    // allocate window and connect common listeners
    SetWindow( m_LabelWindow = new LabelWindow( parent, this, m_LabelControl->a_Ellipsize.Get() ) );

    // add listeners
    m_LabelControl->a_HelpText.Changed().AddMethod( this, &LabelWidget::HelpTextChanged );

    // update state of attributes that are not refreshed during Read()
    m_LabelControl->a_HelpText.RaiseChanged();
}

void LabelWidget::DestroyWindow()
{
    HELIUM_ASSERT( m_LabelWindow );

    SetWindow( NULL );

    // remove listeners
    m_LabelControl->a_HelpText.Changed().RemoveMethod( this, &LabelWidget::HelpTextChanged );

    // destroy window
    m_LabelWindow->Destroy();
    m_LabelWindow = NULL;
}

void LabelWidget::Read()
{
    HELIUM_ASSERT( m_Control->IsBound() );

    tstring text;
    m_LabelControl->ReadStringData( text );

    m_LabelWindow->SetLabel( text.c_str() );
}

bool LabelWidget::Write()
{
    return true;
}

void LabelWidget::HelpTextChanged( const Attribute<tstring>::ChangeArgs& args )
{
    m_LabelWindow->SetHelpText( args.m_NewValue );
}