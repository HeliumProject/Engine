#include "Pipeline/SettingsManager.h"

#include "Platform/Debug.h"

using namespace Helium;

REFLECT_DEFINE_OBJECT( SettingsManager );

void SettingsManager::Clean()
{
    // removes any settings types that we lost due to class name changes, etc.
    m_SettingsMap.erase( Reflect::TypeID() );
}
