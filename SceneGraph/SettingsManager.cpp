#include "SceneGraphPch.h"
#include "SettingsManager.h"

HELIUM_DEFINE_CLASS( Helium::Settings );
HELIUM_DEFINE_CLASS( Helium::SettingsManager );

using namespace Helium;

void SettingsManager::PopulateMetaType( Reflect::MetaStruct& comp )
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
    m_SettingsMap.erase( NULL );
}
