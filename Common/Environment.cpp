#include "Environment.h"

bool Nocturnal::GetEnvFlag( const std::string &envVarName )
{
  bool flagValue;
  return GetEnvVar( envVarName, flagValue ) && flagValue;
}
