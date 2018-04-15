#include "Precompile.h"
#include "Editor/Proxy/SceneProxy.h"

#include "Framework/WorldManager.h"

HELIUM_DEFINE_CLASS(Helium::SceneProxy);

void Helium::SceneProxy::Invalidate()
{

}

void Helium::SceneProxy::Initialize(SceneDefinition *pSceneDefinition, SceneProxy *pParentScene)
{
	m_SceneDefinition = pSceneDefinition;
	m_ParentScene = pParentScene;

	if ( !pParentScene )
	{
		// It's a root scene, so make a world
		HELIUM_ASSERT(WorldManager::GetInstance());
		m_World = WorldManager::GetInstance()->CreateWorld(pSceneDefinition);

		// TODO: Wire up entity proxies
	}
}
