#include "EditorPch.h"
#include "Editor/Proxy/WorldProxy.h"

#include "Framework/WorldManager.h"

REFLECT_DEFINE_OBJECT(Helium::WorldProxy);

void Helium::WorldProxy::Invalidate()
{
    
}

void Helium::WorldProxy::Initialize( WorldDefinition * pWorldDefinition )
{
    m_WorldStrongPtr = WorldManager::GetStaticInstance().CreateWorld(pWorldDefinition);
    m_WorldWeakPtr = m_WorldStrongPtr;
}