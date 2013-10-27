#include "SceneGraphPch.h"
#include "Resource.h"
#include "Statistics.h"

#include "IndexResource.h"
#include "VertexResource.h"

#include "Foundation/Profile.h"
#include "Foundation/Log.h"

#include <algorithm>

using namespace Helium::SceneGraph;

void Resource::Create()
{
	if ( !m_IsCreated )
	{
		m_IsCreated = true;

		if (Allocate())
		{
			Populate();
		}
	}
}

void Resource::Delete()
{
	if ( m_IsCreated )
	{
		m_IsCreated = false;

		Release();
	}
}

void Resource::Update()
{
	if (m_IsCreated)
	{
		if (m_IsDirty || !IsDynamic())
		{
			Delete();
			Create();
		}
		else
		{
			Populate();
		}
	}
	else
	{
		Create();
	}
}

void Resource::Populate()
{
	if (m_Populator.Valid())
	{
		PopulateArgs args ( m_Type, m_BaseIndex = 0, Lock() );

		if (args.m_Buffer)
		{
			uint32_t offset = args.m_Offset;

			m_Populator.Invoke( &args );

			HELIUM_ASSERT( args.m_Offset == ( offset + GetElementSize() * m_ElementCount ) );

			Unlock();
		}
		else
		{
			HELIUM_BREAK(); // this is BAD
		}
	}
}
