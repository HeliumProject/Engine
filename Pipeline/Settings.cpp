#include "Settings.h"

using namespace Helium;

REFLECT_DEFINE_CLASS( Settings );

void Settings::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddField( &Settings::m_UserVisible, TXT( "m_UserVisible" ), Reflect::FieldFlags::Hide );
}
