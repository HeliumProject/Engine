#include "Pipeline/SettingsManager.h"

#include "Platform/Debug.h"

using namespace Helium;

REFLECT_DEFINE_OBJECT( SettingsManager );

void SettingsManager::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddStructureField( &This::m_Test, TXT( "m_Test" ) );
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
