#include "Precompile.h"
#include "Selectable.h"

using namespace Helium;
using namespace Helium::Editor;

EDITOR_DEFINE_TYPE( Selectable );

void Selectable::InitializeType()
{
  Reflect::RegisterClass<Selectable>( TXT( "Selectable" ) );
}

void Selectable::CleanupType()
{
  Reflect::UnregisterClass<Selectable>();
}

Selectable::Selectable()
: m_Selected( false )
{

}

Selectable::~Selectable()
{

}

bool Selectable::IsSelectable() const
{
  return true;
}

bool Selectable::IsSelected() const
{
  return m_Selected;
}

void Selectable::SetSelected(bool selected)
{
  m_Selected = selected;
}

void Selectable::ConnectProperties(EnumerateElementArgs& args)
{

}

bool Selectable::ValidatePanel(const tstring& name)
{
  return false;
}