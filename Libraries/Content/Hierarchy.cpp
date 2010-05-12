#include "Hierarchy.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(Hierarchy)

void Hierarchy::EnumerateClass( Reflect::Compositor<Hierarchy>& comp )
{
  Reflect::Field* fieldObjects = comp.AddField( &Hierarchy::m_Objects, "m_Objects" );
  Reflect::Field* fieldParents = comp.AddField( &Hierarchy::m_Parents, "m_Parents" );
}


void Content::Hierarchy::Reserve(unsigned p_NodeCount)
{
	m_Objects.reserve(p_NodeCount);
	m_Parents.reserve(p_NodeCount);
}

void Content::Hierarchy::AddParentID(UniqueID::TUID p_Node, UniqueID::TUID p_Parent)
{
	m_Objects.push_back(p_Node);
	m_Parents.push_back(p_Parent);
}

UniqueID::TUID Content::Hierarchy::GetParentID(UniqueID::TUID p_Node)
{
	for (unsigned int i=0; i<m_Objects.size(); i++)
	{
		if (m_Objects[i] == p_Node)
			return m_Parents[i];
	}

	// it has no parent, IE its a root object
	return UniqueID::TUID::Null;
}