#pragma once

#include "API.h"

#include "TUID/TUID.h"

namespace Maya
{

  namespace ShaderTypes
  {
    enum ShaderType
    {
      Unknown  = 0,

      Blinn    = 1,
      Lambert,
    };
  }
  typedef ShaderTypes::ShaderType ShaderType;
  
  void MAYASHADERMANAGER_API CreateShader( std::string& shaderFilename, const std::string& shaderName, const std::string& colorMapPath, const std::string& normalMapPath, const std::string& expensiveMapPath );
  void MAYASHADERMANAGER_API Setup2dTextureNode( MObject &textureMapNode );

  void MAYASHADERMANAGER_API SetShaderId( MObject &object, tuid shaderId, const std::string& shaderName = std::string( "" ) );
  tuid MAYASHADERMANAGER_API GetShaderId( const MObject &object, bool getTopShaderId = false );

  void MAYASHADERMANAGER_API LoadShader( const ShaderType shaderType, tuid shaderId, const std::string& shaderFilename = std::string( "" ) );

  bool MAYASHADERMANAGER_API HasAlpha( const tuid shaderId );

  void MAYASHADERMANAGER_API CleanupShaders();

  void MAYASHADERMANAGER_API UpdateShaders();
  void MAYASHADERMANAGER_API UpdateTexturePaths( MObject &shaderNode );
  void MAYASHADERMANAGER_API UpdateTexturePaths( MObject& shaderNode, const std::string& colorMapFilePath, const std::string& normalMapFilePath );
  void MAYASHADERMANAGER_API GetTexturePaths( MObject& shaderNode, std::string& colorMapFilePath, std::string& normalMapFilePath );

  void MAYASHADERMANAGER_API GetShaders( const std::string& nameFilter, V_string& shaderNames );
  void MAYASHADERMANAGER_API GetMayaSurfaceShaderNames( tuid shaderID, MStringArray& shaderNames );
  void MAYASHADERMANAGER_API GetUniqueShaderIDs( MStringArray& shaderIDStrs );

  MStatus MAYASHADERMANAGER_API ShowShaderBrowser();
  void MAYASHADERMANAGER_API HideShaderBrowserCallBack( void* clientData );

  MStatus MAYASHADERMANAGER_API CreateShaderWizard();

  //
  // API's for dual-layer shaders
  //

  bool MAYASHADERMANAGER_API IsDualLayerShader( MObject& shaderNode );

  void MAYASHADERMANAGER_API GetDualTexturePaths( MObject& shaderNode, std::string& baseMap, std::string& topMap );
  void MAYASHADERMANAGER_API SetDualTexturePaths( MObject& shaderNode, const std::string& baseMap, const std::string& topMap );
  void MAYASHADERMANAGER_API GetBlendTexturePath( MObject& shaderNode, std::string& blendMap );
  void MAYASHADERMANAGER_API SetBlendTexturePath( MObject& shaderNode, const std::string& blendMap );
   
  class MAYASHADERMANAGER_API ShaderManager : public MPxCommand
  {
  public:
    ShaderManager();
    virtual ~ShaderManager() { }

    static void*   creator();
    static MSyntax commandSyntax();

    void           DisplayUsage() const;

    MStatus doIt( const MArgList &args );
  };


} // namespace Maya