#include "Environment.h"

bool Nocturnal::GetEnvFlag( const tstring &envVarName )
{
    bool flagValue;
    return GetEnvVar( envVarName, flagValue ) && flagValue;
}
