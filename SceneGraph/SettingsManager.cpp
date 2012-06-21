#include "SceneGraphPch.h"
#include "SettingsManager.h"

#include "Platform/Debug.h"

REFLECT_DEFINE_OBJECT( Helium::Settings );
REFLECT_DEFINE_OBJECT( Helium::SettingsManager );

using namespace Helium;

void SettingsManager::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &SettingsManager::m_SettingsMap, TXT( "m_SettingsMap" ), Reflect::FieldFlags::Hide );
}

SettingsManager::SettingsManager()
{
}

SettingsManager::~SettingsManager()
{
}

void SettingsManager::Clean()
{
    // removes any settings types that we lost due to class name changes, etc.
    m_SettingsMap.erase( Reflect::TypeID() );
}
