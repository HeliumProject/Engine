#include "Core/Content/Nodes/Layer.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(Layer)

void Layer::EnumerateClass( Reflect::Compositor<Layer>& comp )
{
  Reflect::Field* fieldVisible    = comp.AddField( &Layer::m_Visible, "m_Visible" );
  Reflect::Field* fieldSelectable = comp.AddField( &Layer::m_Selectable, "m_Selectable" );
  Reflect::Field* fieldMembers    = comp.AddField( &Layer::m_Members, "m_Members" );
  Reflect::Field* fieldColor      = comp.AddField( &Layer::m_Color, "m_Color" );
}

Layer::Layer()
: SceneNode()
, m_Visible( true )
, m_Selectable( true )
, m_Color( 255 )
{
}

Layer::Layer( Helium::TUID& id )
: SceneNode( id )
, m_Visible( true )
, m_Selectable( true )
, m_Color( 255 )
{
}