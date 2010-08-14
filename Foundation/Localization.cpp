#include "Localization.h"

using namespace Helium;

namespace Helium
{
    namespace Localization
    {
        static Localizer* s_GlobalLocalizer = NULL;
    }
}

Localization::Localizer& Localization::GlobalLocalizer()
{
    if ( !s_GlobalLocalizer )
    {
        s_GlobalLocalizer = new Localizer();
    }
    return *s_GlobalLocalizer;
}

void Localization::Initialize()
{
    if ( !s_GlobalLocalizer )
    {
        s_GlobalLocalizer = new Localizer();
    }
}

void Localization::Cleanup()
{
    if ( s_GlobalLocalizer )
    {
        delete s_GlobalLocalizer;
    }
}

const tstring& Localization::GetString( const TableId& tableId, const StringId& stringId, const LanguageId& languageId )
{
    return GlobalLocalizer().GetString( tableId, stringId, languageId );
}
