#include "Precompile.h" 

#include "LooseGraphShaderAsset.h"

#include "AssetManager.h"
#include "AttributeExistenceCommand.h"
#include "PersistentDataFactory.h"

#include "File/Manager.h"
#include "Console/Console.h"
#include "Inspect/Control.h"
#include "Inspect/Container.h"
#include "Finder/AssetSpecs.h"
#include "FileSystem/FileSystem.h"
#include "rpc/interfaces/rpc_lunaview.h"
#include "AssetEditor.h" 

#include "BuilderUtil/GraphShaderBuildStrategy.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::LooseGraphShaderAsset );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::LooseGraphShaderAsset types.
// 
void LooseGraphShaderAsset::InitializeType()
{
  Reflect::RegisterClass<Luna::LooseGraphShaderAsset>( "Luna::LooseGraphShaderAsset" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::LooseGraphShaderAsset >(), &Luna::LooseGraphShaderAsset::Create );
}

void LooseGraphShaderAsset::CleanupType()
{
  Reflect::UnregisterClass<Luna::LooseGraphShaderAsset>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function.
// 
Luna::PersistentDataPtr LooseGraphShaderAsset::Create( Reflect::Element* lgsClass, Luna::AssetManager* manager )
{
  return new Luna::LooseGraphShaderAsset( Reflect::AssertCast< Asset::LooseGraphShaderAsset >( lgsClass ), manager );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a property has changed. 
// 
void LooseGraphShaderAsset::Changed( Inspect::Control* control )
{
  __super::Changed( control );

  Asset::LooseGraphShaderAssetPtr lgsPkg = GetPackage< Asset::LooseGraphShaderAsset>();
 
  if ( control && control->HasType( Reflect::GetType<Inspect::Button>() ) )
  {
    Inspect::StringData* data = Inspect::CastData< Inspect::StringData, Inspect::DataTypes::String >( control->GetData() );

    std::string path;
    data->Get( path );

    //Is it a graph shader?
    if(FileSystem::HasExtension( path, FinderSpecs::Extension::GRAPH_SHADER.GetValue() ))
    {
      std::string errorMsg;
      bool        result  = BuilderUtil::GraphShaderBuildStrategy::ValidateFileType(path, 
                                                                                    "CustomIGLooseShader",
                                                                                    errorMsg);      
      if(result == false)
      {
        errorMsg  = errorMsg + " Canceling operation";
        wxMessageBox( errorMsg.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetAssetManager()->GetAssetEditor() );
        lgsPkg->m_GraphFile = TUID::Null;
        data->Set("");
      }
    }
  }
}