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

  void MAYASHADERMANAGER_API SetShader( MObject &object, const std::string& shaderPath );
  std::string MAYASHADERMANAGER_API GetShader( const MObject &object );

  void MAYASHADERMANAGER_API LoadShader( const ShaderType shaderType, const std::string& shaderFile );

  bool MAYASHADERMANAGER_API HasAlpha( const std::string& shaderPath );

  void MAYASHADERMANAGER_API CleanupShaders();

  void MAYASHADERMANAGER_API UpdateShaders();
  void MAYASHADERMANAGER_API UpdateTexturePaths( MObject &shaderNode );
  void MAYASHADERMANAGER_API UpdateTexturePaths( MObject& shaderNode, const std::string& colorMapFilePath, const std::string& normalMapFilePath );
  void MAYASHADERMANAGER_API GetTexturePaths( MObject& shaderNode, std::string& colorMapFilePath, std::string& normalMapFilePath );

  void MAYASHADERMANAGER_API GetShaders( const std::string& nameFilter, V_string& shaderNames );
  void MAYASHADERMANAGER_API GetMayaSurfaceShaderNames( tuid shaderID, MStringArray& shaderNames );
  void MAYASHADERMANAGER_API GetUniqueShaders( MStringArray& shaders );

  MStatus MAYASHADERMANAGER_API ShowShaderBrowser();
  void MAYASHADERMANAGER_API HideShaderBrowserCallBack( void* clientData );

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