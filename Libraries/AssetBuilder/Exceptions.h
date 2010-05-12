#pragma once

#include "Asset/AssetClass.h"
#include "Asset/ShaderAsset.h"
#include "Common/Exception.h"
#include "File/Manager.h"
#include "Reflect/Registry.h"
#include "TUID/tuid.h"


namespace AssetBuilder
{
  class Exception : public Nocturnal::Exception
  {
  public:
    Exception( const char *msgFormat, ... )
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }

  protected:
    Exception() throw() {} // hide default c_tor
  };

  class InvalidAssetClassException : public Exception
  {
  public:
    InvalidAssetClassException( tuid assetClassID, const char* assetType )
      : Exception( "AssetClass ["TUID_HEX_FORMAT"] is not valid for use as: %s", assetClassID, assetType )
    {

    }
  };

  class InvalidShaderUsageException : public Exception
  {
  public:
    InvalidShaderUsageException( tuid shaderID, Asset::AssetClass* assetWithShader, bool isShaderFromRemap = false )
      : Exception( "InvalidShaderUsageException" )
    {
      Asset::ShaderAssetPtr shader = Asset::AssetClass::GetAssetClass< Asset::ShaderAsset >( shaderID );

      std::string engineTypeStr;
      const Reflect::Enumeration* enumInfo = Reflect::Registry::GetInstance()->GetEnumeration( "EngineType" );
      if ( enumInfo )
      {
        enumInfo->GetElementLabel( assetWithShader->GetEngineType(), engineTypeStr );
      }

      // Invalid shader (SHADER_NAME) applied to 'ASSET': 'SHADER_TYPE' not allowed on engine type 'ENGINE_TYPE'
      std::stringstream msg;
      msg << "Invalid shader (" << shader->GetFullName() << ") applied to '" << assetWithShader->GetShortName() << "': '" << shader->GetClass()->m_UIName << "' not allowed on engine type '" << engineTypeStr << "'.";
      if ( isShaderFromRemap )
      {
        msg << " Check the Shader Group Attribute on this asset for the remapped shader.";
      }

      SetMessage( msg.str().c_str() );
    }

  };
}