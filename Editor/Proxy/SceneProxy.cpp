#include "EditorPch.h"
#include "Editor/Proxy/SceneProxy.h"

#include "Framework/WorldManager.h"

REFLECT_DEFINE_OBJECT(Helium::SceneProxy);

void Helium::SceneProxy::Invalidate()
{
    
}

void Helium::SceneProxy::Initialize( SceneDefinition * pSceneDefinition )
{
	m_SceneDefinition = pSceneDefinition;
}