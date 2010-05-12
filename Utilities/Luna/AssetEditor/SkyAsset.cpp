#include "Precompile.h"
#include "SkyAsset.h"

#include "AssetManager.h"
#include "AttributeExistenceCommand.h"
#include "PersistentDataFactory.h"

#include "Asset/SkyShellAttribute.h"
#include "File/Manager.h"
#include "Console/Console.h"
#include "Inspect/Control.h"
#include "Inspect/Container.h"
#include "Finder/AssetSpecs.h"
#include "FileSystem/FileSystem.h"
#include "rpc/interfaces/rpc_lunaview.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::SkyAsset );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::SkyAsset types.
// 
void SkyAsset::InitializeType()
{
  Reflect::RegisterClass<Luna::SkyAsset>( "Luna::SkyAsset" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::SkyAsset >(), &SkyAsset::Create );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::SkyAsset types.
// 
void SkyAsset::CleanupType()
{
  Reflect::UnregisterClass<Luna::SkyAsset>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function.
// 
Luna::PersistentDataPtr SkyAsset::Create( Reflect::Element* skyClass, Luna::AssetManager* manager )
{
  return new Luna::SkyAsset( Reflect::AssertCast< Asset::SkyAsset >( skyClass ), manager );
}


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SkyAsset::SkyAsset( Asset::SkyAsset* skyClass, Luna::AssetManager* manager )
: Luna::AssetClass( skyClass, manager )
{
  AssetBuilder::V_IBuilder builders;
  BuilderInterface::AllocateBuilders( skyClass->GetEngineType(), builders );

  if ( builders.size() != 1 )
  {
    throw Nocturnal::Exception( "More than one (or no) builder was allocated for a Sky!" );
  }

  m_SkyBuilder = static_cast< AssetBuilder::ISkyBuilder* >( builders.front().Ptr() );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SkyAsset::~SkyAsset()
{

}

///////////////////////////////////////////////////////////////////////////////
// Called after an object is deserialized.  Automatically synchronizes the sky
// shells.
// 
void SkyAsset::Unpack()
{
  __super::Unpack();

  SyncShells();
}

///////////////////////////////////////////////////////////////////////////////
// Adds additional, sky specfic, context menu items.
// 
void SkyAsset::PopulateContextMenu( ContextMenuItemSet& menu )
{
  __super::PopulateContextMenu( menu );

  menu.AppendSeparator();
  ContextMenuItemPtr menuItem = new ContextMenuItem( "Sync Sky Shells" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( this, &SkyAsset::OnSyncShells ) );
  menu.AppendItem( menuItem );
}


///////////////////////////////////////////////////////////////////////////////
// Synchronizes the SkyShellAttribute with the actual sky shells found in the
// Maya data for this sky.  If this object does not have a SkyShellAttribute,
// one will be added.
// 
void SkyAsset::SyncShells()
{
  Asset::SkyAssetPtr skyPkg = Reflect::ObjectCast< Asset::SkyAsset >( GetPackage< Asset::SkyAsset >()->Clone() );

  try 
  {
    m_SkyBuilder->SyncShells( skyPkg );
  }
  catch ( const Nocturnal::Exception& e )
  {
    Console::Debug( "Unable to sync sky shells: %s\n", e.what() );
  }

  Luna::AttributeWrapperPtr oldShellAttrib = FindAttribute( Reflect::GetType< Asset::SkyShellAttribute >() );
  if ( oldShellAttrib.ReferencesObject() )
  {
    Undo::ExistenceCommandPtr cmd = new AttributeExistenceCommand( Undo::ExistenceActions::Remove, this, oldShellAttrib );
    // Not undoable, throw away the command
  }

  Attribute::AttributePtr attrib = skyPkg->GetAttribute( Reflect::GetType< Asset::SkyShellAttribute >() );
  if ( attrib.ReferencesObject() )
  {
    // We have to clone the attribute since we want to add it to a different collection
    Attribute::AttributePtr attribClone = Reflect::ObjectCast< Attribute::AttributeBase >( attrib->Clone() );
    Luna::AttributeWrapperPtr attrWrapper = PersistentDataFactory::GetInstance()->CreateTyped< Luna::AttributeWrapper >( attribClone, GetAssetManager() );
    Undo::ExistenceCommandPtr cmd = new AttributeExistenceCommand( Undo::ExistenceActions::Add, this, attrWrapper );
    // Not undoable, throw away the command
  }

  Changed( NULL );

  m_AssetManager->ClearUndoQueue();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a property has changed.  Updates the sky on the devkit if
// applicable.
// 
void SkyAsset::Changed( Inspect::Control* control )
{
  __super::Changed( control );

  Asset::SkyAssetPtr skyPkg = GetPackage< Asset::SkyAsset >();

  // If the Refresh button was pressed, figure out which textures where changed, 
  // and update them on the devkit.


  //abezrati: allow the artists to remove sky textures by deleting the UI path
  if ( control )
  {
    Inspect::StringData* data = Inspect::CastData< Inspect::StringData, Inspect::DataTypes::String >( control->GetData() );

    std::string path;
    data->Get( path );

    //Is it a loose graph shader?
    if(FileSystem::HasExtension( path, FinderSpecs::Asset::LOOSE_GRAPH_SHADER_DECORATION.GetDecoration() ))
    {
      Asset::LooseGraphShaderAssetPtr lgsPtr;
      tuid                            lgsFileId;
      
      try
      {
        lgsFileId = File::GlobalManager().GetID( path ); 
        lgsPtr    = Asset::AssetClass::GetAssetClass< Asset::LooseGraphShaderAsset >( lgsFileId );
         
        if(lgsPtr->m_GraphFile != TUID::Null)
        {
          SkyLooseGraphShaderChangedArgs lgsChangeArgs;
          lgsChangeArgs.m_SkyClass              = skyPkg;
          lgsChangeArgs.m_SkyBuilder            = m_SkyBuilder;  
          lgsChangeArgs.m_LooseGraphShaderClass = lgsPtr;
          m_SkyLooseGraphShaderChanged.Raise( lgsChangeArgs );     
        }
      }
      catch ( const Nocturnal::Exception& )
      {
        // do nothing
      }
    }
    //Not a loose graphshader
    else
    {
      tuid textureID = TUID::Null;
      try
      {
        textureID = File::GlobalManager().GetID( path );
      }
      catch ( const Nocturnal::Exception& )
      {
        // do nothing
      }

      //abezrati: NULL textures are ok
      if ( textureID == TUID::Null )
      {
        //Clear out the path
        path.clear();
      }

      {
        SkyTextureChangedArgs texChangeArgs;
        texChangeArgs.m_SkyClass = skyPkg;
        texChangeArgs.m_SkyBuilder = m_SkyBuilder;
        texChangeArgs.m_TexturePath = path;

        if ( skyPkg->m_HighFrequencyData->m_CloudTextureId == textureID )
        {
          texChangeArgs.m_TextureType = RPC::SkyTextures::HighFreqCloudTexture;
          m_SkyTextureChanged.Raise( texChangeArgs );
        }

        if ( skyPkg->m_LowFrequencyData->m_CloudTextureId == textureID )
        {
          texChangeArgs.m_TextureType = RPC::SkyTextures::LowFreqCloudTexture;
          m_SkyTextureChanged.Raise( texChangeArgs );
        }

        if ( skyPkg->m_TurbulanceTextureID == textureID )
        {
          texChangeArgs.m_TextureType = RPC::SkyTextures::TurbulanceTexture;
          m_SkyTextureChanged.Raise( texChangeArgs );
        }
      }
    }
  }

  SkyChangedArgs changeArgs;
  changeArgs.m_SkyClass = skyPkg;
  changeArgs.m_SkyBuilder = m_SkyBuilder;
  m_SkyChanged.Raise( changeArgs );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the "sync sky shells" menu item is selected.
// 
void SkyAsset::OnSyncShells( const ContextMenuArgsPtr& args )
{
  if ( m_AssetManager->IsEditable( this ) )
  {
    SyncShells();
  }
}
