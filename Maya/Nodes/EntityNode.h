#pragma once

#include "API.h"
#include "EntityInstanceNode.h"

#include "Export/ExportScene.h"

#include <maya/MPxTransform.h>

namespace Helium
{
    class EntityNode;

    typedef std::map< u64, EntityNode* > M_IdClassTransform;

    static const char kUnselectInstanceData[] = {"igUnselectInstanceData"};

    //
    // This coallates EntityInstanceNode objects for scene tidyness,
    //  and makes wrangling instances in the code easier
    //

    class MAYA_API EntityGroupNode : public MPxTransform 
    {
    public:
        static const char* s_TypeName;
        static const MTypeId s_TypeID;  

        static void * Creator()
        {
            return new EntityGroupNode();  
        }

        static MStatus Initialize()
        {
            return MS::kSuccess;
        }
    };

    class MAYA_API EntityNode : public MPxTransform 
    {
        friend EntityInstanceNode;

    public:
        // type information
        static const char* s_TypeName;
        static const MTypeId s_TypeID;  

        static EntityNode    Null;

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
        Helium::Path m_AssetPath;

        // client data for each instance of this art class
        M_EntityNode m_Instances;

    public:
        EntityNode();
        ~EntityNode();

        static void * Creator();
        static MStatus Initialize();	
        virtual void postConstructor();

        //
        // Since instances of EntityNode are singlton on asset ids, we can just compare those ids
        //

        bool operator == ( const EntityNode& rhs ) const
        {
            return m_AssetPath.Hash() == rhs.m_AssetPath.Hash();
        }  

        bool operator != ( const EntityNode& rhs ) const
        {
            return m_AssetPath.Hash() != rhs.m_AssetPath.Hash();
        }

        //
        // EntityNode manager API
        //

        // (re)import art for all EntityAssetNodes
        static void LoadAllArt();

        // delete all imported art data
        static void UnloadAllArt();

        // WTF does this do?  what is it for? -Geoff
        static void FlattenInstances();

        // get a reference to the specified EntityNode, create it if it doesn't exist
        static EntityNode& Get( const Helium::Path& path, bool createIfNotExisting = true );

        // get/create an EntityNode, and an EntityInstanceNode for the given content Entity
        static std::pair< EntityNode*, EntityInstanceNode* > CreateInstance( const Asset::EntityInstancePtr& entity );

        // called when removing an instance
        static void RemoveInstance( EntityInstanceNode* entityNode );

        //
        // EntityNode members
        //

        // this does some path stuff
        void GetArtFilePath( MString& artFilePath );
        void SetArtFilePath( const MString& artFilePath );

        // import the EntityNode's maya/art data from the backing asset class's art file.
        void LoadArt();

        // delete the EntityNode's maya/art data and clear all the EntityNode's instances 
        void UnloadArt();

        // adds and retrieves an MObject for each DG node that should be associated with the art class
        void AddImportNode( const MObject& object );
        void GetImportNodes( MObjectArray& importNodes );

        // add instanced copy of the specified object to the EntityNode's instances
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

}