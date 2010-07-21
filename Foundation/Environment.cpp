#include "Environment.h"

#include "Platform/Assert.h"

#pragma TODO("Deprecate GetEnvVar, GetEnvFlag")

bool Nocturnal::GetEnvFlag( const tstring &envVarName )
{
    bool flagValue;
    return GetEnvVar( envVarName, flagValue ) && flagValue;
}
