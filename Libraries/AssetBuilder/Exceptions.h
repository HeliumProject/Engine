#pragma once

#include "Asset/AssetClass.h"
#include "Asset/ShaderAsset.h"
#include "Common/Exception.h"
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
        InvalidShaderUsageException( const std::string& shaderPath, Asset::AssetClass* assetWithShader, bool isShaderFromRemap = false )
            : Exception( "InvalidShaderUsageException" )
        {
            Asset::ShaderAssetPtr shader = Asset::AssetClass::LoadAssetClass< Asset::ShaderAsset >( shaderPath );

            std::string assetTypeStr;
            //const Reflect::Enumeration* enumInfo = Reflect::Registry::GetInstance()->GetEnumeration( "AssetType" );
            //if ( enumInfo )
            //{
            //    enumInfo->GetElementLabel( assetWithShader->GetAssetType(), assetTypeStr );
            //}

            // Invalid shader (SHADER_NAME) applied to 'ASSET': 'SHADER_TYPE' not allowed on engine type 'ENGINE_TYPE'
            std::stringstream msg;
            msg << "Invalid shader (" << shader->GetFullName() << ") applied to '" << assetWithShader->GetShortName() << "': '" << shader->GetClass()->m_UIName << "' not allowed on engine type '" << assetTypeStr << "'.";
            if ( isShaderFromRemap )
            {
                msg << " Check the Shader Group Attribute on this asset for the remapped shader.";
            }

            SetMessage( msg.str().c_str() );
        }

    };
}