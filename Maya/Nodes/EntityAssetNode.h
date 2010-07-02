#pragma once

#include "API.h"
#include "EntityNode.h"

#include "Export/ExportScene.h"

#include <maya/MPxTransform.h>

class EntityAssetNode;

typedef std::map< u64, EntityAssetNode* > M_IdClassTransform;

static const char kUnselectInstanceData[] = {"igUnselectInstanceData"};

class MAYA_API EntityAssetNode : public MPxTransform 
{
  friend EntityNode;

public:
  // type information
  static const char* s_TypeName;
  static const MTypeId s_TypeID;  

  static EntityAssetNode    Null;

  // callbacks
  static MIntArray s_CallbackIDs; 
  static MStatus AddCallbacks();
  static MStatus RemoveCallbacks();

  // attributes
  static MObject s_ArtFilePath;
  static MObject m_ImportNodes;
  static MObject s_EntityAssetGroup;

  // static data
  static M_IdClassTransform   s_ClassTransformsMap;
  static MCallbackId          s_EditNodeAddedCBId;
  static MCallbackId          s_ImportNodeAddedCBId; 
  static bool                 s_DoRemoveNodeCallback;

private:
    Nocturnal::Path m_AssetPath;

  // client data for each instance of this art class
  M_EntityNode m_Instances;

public:
  EntityAssetNode();
  ~EntityAssetNode();

  static void * Creator();
  static MStatus Initialize();	
  virtual void postConstructor();

  //
  // Since instances of EntityAssetNode are singlton on asset ids, we can just compare those ids
  //

  bool operator == ( const EntityAssetNode& rhs ) const
  {
      return m_AssetPath.Hash() == rhs.m_AssetPath.Hash();
  }  

  bool operator != ( const EntityAssetNode& rhs ) const
  {
      return m_AssetPath.Hash() != rhs.m_AssetPath.Hash();
  }

  //
  // EntityAssetNode manager API
  //

  // (re)import art for all EntityAssetNodes
  static void LoadAllArt();

  // delete all imported art data
  static void UnloadAllArt();

  // WTF does this do?  what is it for? -Geoff
  static void FlattenInstances();

  // get a reference to the specified EntityAssetNode, create it if it doesn't exist
  static EntityAssetNode& Get( const Nocturnal::Path& path, bool createIfNotExisting = true );

  // get/create an EntityAssetNode, and an EntityNode for the given content Entity
  static std::pair< EntityAssetNode*, EntityNode* > CreateInstance( const Asset::EntityInstancePtr& entity );

  // called when removing an instance
  static void RemoveInstance( EntityNode* entityNode );
  
  //
  // EntityAssetNode members
  //

  // this does some path stuff
  void GetArtFilePath( MString& artFilePath );
  void SetArtFilePath( const MString& artFilePath );

  // import the EntityAssetNode's maya/art data from the backing asset class's art file.
  void LoadArt();

  // delete the EntityAssetNode's maya/art data and clear all the EntityAssetNode's instances 
  void UnloadArt();

  // adds and retrieves an MObject for each DG node that should be associated with the art class
  void AddImportNode( const MObject& object );
  void GetImportNodes( MObjectArray& importNodes );

  // add instanced copy of the specified object to the EntityAssetNode's instances
  const M_EntityNode& GetInstances()
  {
    return m_Instances;
  }
  void AddToInstances( MObject &addedNode );
  void ClearInstances();
  void ReloadInstances();

  // used to prohibit the user from selecting things we don't want them to select (essentially a lock and ease of use mechanism)
  void Unselect( MSelectionList& list );
  static void UnselectAll( MSelectionList& list );

  // scene callback for keeping this data up to date, called before open and after new
  static void FlushCallback( void* clientData );

  // do logic to keep our separate instance data up to date when nodes are added or removed
  static void NodeAddedCallback( MObject& node, void* clientData );
  static void NodeRemovedCallback( MObject& node, void* clientData );

  // build the list of nodes that were imported
  static void ImportNodeAddedCallback( MObject &addedNode, void* clientData );

  // User event callback for unselecting all instance data
  static void UnselectAllCallback( void* clientData );
};

