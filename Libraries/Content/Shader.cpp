#include "Shader.h"
#include "ContentVisitor.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/ContentSpecs.h"
#include "Finder/ShaderSpecs.h"
#include "Finder/AssetSpecs.h"
#include "TUID/TUID.h"
#include "Attribute/AttributeHandle.h"
#include "Console/Console.h"
#include "rcs/rcs.h"

using namespace Reflect;
using namespace Content;
using namespace Attribute; 

REFLECT_DEFINE_CLASS(Shader)

void Shader::EnumerateClass( Reflect::Compositor<Shader>& comp )
{
  Reflect::Field* fieldWrapU = comp.AddField( &Shader::m_WrapU, "m_WrapU" );
  Reflect::Field* fieldWrapV = comp.AddField( &Shader::m_WrapV, "m_WrapV" );
  Reflect::Field* fieldRepeatU = comp.AddField( &Shader::m_RepeatU, "m_RepeatU" );
  Reflect::Field* fieldRepeatV = comp.AddField( &Shader::m_RepeatV, "m_RepeatV" );
  Reflect::Field* fieldBaseColor = comp.AddField( &Shader::m_BaseColor, "m_BaseColor" );
  Reflect::Field* fieldBaseTextureFileID = comp.AddField( &Shader::m_BaseTextureFileID, "m_BaseTextureFileID" );
}

static const char* s_ShaderAssetIdAttrName   = "shaderAssetId";

bool Shader::ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName)
{
  if ( fieldName == "m_BaseTextureFilePath" )
  {
    std::string textureFilePath;
    Serializer::GetValue( Reflect::AssertCast<Serializer>(element), textureFilePath );
    Finder::StripAnyProjectAssets(textureFilePath);

    std::string correctPath = Finder::ProjectAssets();
    FileSystem::AppendPath( correctPath, textureFilePath );

    if (FileSystem::Exists(correctPath))
    {
      m_BaseTextureFileID = File::GlobalManager().Open( correctPath );
    }

    return true;
  }

  return __super::ProcessComponent( element, fieldName );
}

std::string Shader::GetBaseTextureFilePath() const
{
  if ( m_BaseTextureFileID != TUID::Null )
  {
    return File::GlobalManager().GetPath( m_BaseTextureFileID );
  }
  return "";
}

void Shader::Host(ContentVisitor* visitor)
{
  visitor->VisitShader(this); 
}

tuid Shader::GetAssetID() const
{
  tuid assetId = TUID::Null;

  Reflect::ElementPtr shaderIDData = GetComponent( s_ShaderAssetIdAttrName, Reflect::GetType<StringSerializer>() );
  if ( shaderIDData.ReferencesObject() )
  {
    std::string assetIdStr;
    Serializer::GetValue( AssertCast<Serializer>(shaderIDData), assetIdStr );
    if ( !assetIdStr.empty() )
    {
      std::istringstream idStream ( assetIdStr );
      idStream >> std::hex >> assetId;
    }
  }

  return assetId;
}

void Shader::SetAssetID(tuid assetID)
{
  std::stringstream assetIdStr;
  assetIdStr << TUID::HexFormat << assetID;

  SetComponent( s_ShaderAssetIdAttrName, Serializer::Create<std::string>( assetIdStr.str() ) );
}