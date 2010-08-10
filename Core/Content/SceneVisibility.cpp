#include "SceneVisibility.h"
#include "Foundation/TUID.h"

using namespace Helium;
using namespace Helium::Content; 

REFLECT_DEFINE_CLASS( SceneVisibility );

void SceneVisibility::EnumerateClass( Reflect::Compositor<SceneVisibility>& comp )
{
  Reflect::Field* fieldNodeVisibility = comp.AddField( &SceneVisibility::m_NodeVisibility, "m_NodeVisibility" );
}

SceneVisibility::SceneVisibility()
: m_NodeDefaults( new NodeVisibility() )
{


}

NodeVisibilityPtr SceneVisibility::GetVisibility(tuid nodeId,tuid fallbackId)
{
  HELIUM_ASSERT(m_NodeDefaults); 

  M_TuidVisibility::iterator itr = m_NodeVisibility.find(nodeId); 

  // check the saved settings
  if( itr!= m_NodeVisibility.end())
  {
    return itr->second;
  }

  // check the temp map
  itr = m_TempVisibility.find(nodeId); 

  if( itr != m_TempVisibility.end())
  {
    return itr->second;
  }

  // check for a fallback
  if(fallbackId != Helium::TUID::Null)
  {
    itr = m_NodeVisibility.find(fallbackId); 

    // check the saved settings
    if( itr != m_NodeVisibility.end())
    {
      // init from fallback visibility
      NodeVisibilityPtr nodeVis = Reflect::AssertCast<NodeVisibility>( (*itr).second->Clone() ); 
      itr = m_NodeVisibility.insert( std::make_pair(nodeId, nodeVis) ).first; 
      return itr->second;
    }

    // check the temp map
    itr = m_TempVisibility.find(fallbackId); 

    if( itr != m_TempVisibility.end())
    {
      NodeVisibilityPtr nodeVis = Reflect::AssertCast<NodeVisibility>( (*itr).second->Clone() ); 
      itr = m_TempVisibility.insert( std::make_pair(nodeId, nodeVis) ).first; 
      return itr->second;
    } 
  }

  // if we still haven't found it, create one and make it temp
  // use our defaults to initialize the new visibility. 
  NodeVisibilityPtr nodeVis = Reflect::AssertCast<NodeVisibility>( m_NodeDefaults->Clone() ); 
  itr = m_TempVisibility.insert( std::make_pair(nodeId, nodeVis) ).first; 

  return itr->second; 
}

// move node from temp to permanent
//
void SceneVisibility::ActivateNode(tuid nodeId)
{
  M_TuidVisibility::iterator itr = m_TempVisibility.find(nodeId); 
  
  // if we didn't find it in the temp, we either have it in permanent 
  // or we have no information; this function does not add new visibility info
  
  if(itr == m_TempVisibility.end())
  {
    return; 
  }

  m_NodeVisibility.insert( *itr ); 
  m_TempVisibility.erase( itr ); 

}

// move node from permanent to temp
// 
void SceneVisibility::DeactivateNode(tuid nodeId)
{
  M_TuidVisibility::iterator itr = m_NodeVisibility.find(nodeId); 

  // if we didn't find it in the permanent map, we either have it in temp
  // or we have no information; this function does not add new visibility info

  if(itr == m_NodeVisibility.end())
  {
    return; 
  }

  m_TempVisibility.insert( *itr ); 
  m_NodeVisibility.erase( itr ); 
}

void SceneVisibility::SetNodeDefaults(const NodeVisibilityPtr& nodeDefaults)
{
  m_NodeDefaults = nodeDefaults; 
  if(!m_NodeDefaults)
  {
    m_NodeDefaults = new NodeVisibility(); 
  }
}

