#include "Precompile.h"

#include "SceneCallbackData.h"
#include "SceneNode.h"
#include "HierarchyNodeType.h"
#include "InstanceSet.h"

using namespace Luna;

ContextCallbackData::ContextCallbackData():
m_ContextCallbackType( ContextCallbackTypes::All )
, m_NodeType( NULL )
, m_NodeInstance( NULL )
, m_InstanceSet( NULL )
{
}

ContextCallbackData::~ContextCallbackData()
{

}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
GeneralCallbackData::GeneralCallbackData():
m_GeneralData( NULL )
{

}

GeneralCallbackData::~GeneralCallbackData()
{

}

