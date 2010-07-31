#include "NodeVisibility.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS( NodeVisibility );

void NodeVisibility::EnumerateClass( Reflect::Compositor<NodeVisibility>& comp )
{
  Reflect::Field* fieldHiddenNode = comp.AddField( &NodeVisibility::m_HiddenNode, "m_HiddenNode" );
  Reflect::Field* fieldVisibleLayer = comp.AddField( &NodeVisibility::m_VisibleLayer, "m_VisibleLayer" );
  Reflect::Field* fieldShowGeometry = comp.AddField( &NodeVisibility::m_ShowGeometry, "m_ShowGeometry" );
  Reflect::Field* fieldShowBounds = comp.AddField( &NodeVisibility::m_ShowBounds, "m_ShowBounds" );
  Reflect::Field* fieldShowPointer = comp.AddField( &NodeVisibility::m_ShowPointer, "m_ShowPointer" );
}
    
NodeVisibility::NodeVisibility() 
  : m_HiddenNode(false)
  , m_VisibleLayer(true)
  , m_ShowGeometry(false)
  , m_ShowBounds(true)
  , m_ShowPointer(true)
{


}

bool NodeVisibility::GetHiddenNode()
{
  return m_HiddenNode; 
}

void NodeVisibility::SetHiddenNode(bool hidden)
{
  m_HiddenNode = hidden; 
}


bool NodeVisibility::GetVisibleLayer()
{
  return m_VisibleLayer; 
}

void NodeVisibility::SetVisibleLayer(bool visible)
{
  m_VisibleLayer = visible; 
}


bool NodeVisibility::GetShowGeometry()
{
  return m_ShowGeometry; 
}

void NodeVisibility::SetShowGeometry(bool show)
{
  m_ShowGeometry = show; 
}


bool NodeVisibility::GetShowBounds()
{
  return m_ShowBounds; 
}

void NodeVisibility::SetShowBounds(bool show)
{
  m_ShowBounds = show; 
}

bool NodeVisibility::GetShowPointer()
{
  if(!m_ShowGeometry && !m_ShowPointer && !m_ShowBounds)
  {
    return true;
  }
  return m_ShowPointer; 
}

void NodeVisibility::SetShowPointer(bool show)
{
  m_ShowPointer = show; 
  if(!m_ShowPointer && !m_ShowBounds && !m_ShowGeometry)
  {
    m_ShowBounds = true; 
  }
}



