#include "EditorPch.h"
#include "Editor/Proxy/SceneProxy.h"

#include "Framework/WorldManager.h"

HELIUM_DEFINE_CLASS(Helium::SceneProxy);

void Helium::SceneProxy::Invalidate()
{
    
}

void Helium::SceneProxy::Initialize( SceneDefinition *pSceneDefinition, SceneProxy *pParentScene )
{
	m_SceneDefinition = pSceneDefinition;
    m_ParentScene = pParentScene;

    if (!pParentScene)
    {
        // It's a root scene, so make a world
        m_World = WorldManager::GetStaticInstance().CreateWorld(pSceneDefinition);
        
        // TODO: Wire up entity proxies
    }
}
