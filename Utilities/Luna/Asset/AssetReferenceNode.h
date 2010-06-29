#pragma once

#include "AssetNode.h"

#include "Foundation/File/Path.h"

namespace Undo
{
    class Command;
    typedef Nocturnal::SmartPtr< Command > CommandPtr;
}

// Forwards
namespace Asset
{
    struct AssetTypeChangeArgs;
}

namespace Luna
{
    struct PreferenceChangedArgs;
}

namespace Luna
{
    // Forwards
    class Enumerator;
    struct CreatePanelArgs;
    class AssetManager;
    class ComponentContainer;
    class ComponentWrapper;
    class ComponentNode;
    struct AssetLoadArgs;
    struct DocumentChangedArgs;

    /////////////////////////////////////////////////////////////////////////////
    // Node that references an Luna::AssetClass (which may or may not be loaded yet).
    // 
    class AssetReferenceNode : public Luna::AssetNode
    {
    private:
        Luna::AssetClass* m_Asset;
        Nocturnal::Path m_AssetPath;
        Reflect::Element* m_Element;
        const Reflect::Field* m_Field;
        Luna::ComponentContainer* m_ComponentContainer;

    public:
        // Runtime Type Info
        LUNA_DECLARE_TYPE( Luna::AssetReferenceNode, Luna::AssetNode );
        static void InitializeType();
        static void CleanupType();

    public:
        AssetReferenceNode( Luna::AssetManager* manager, const Nocturnal::Path& assetPath, const Reflect::Field* field );
        virtual ~AssetReferenceNode();
        virtual Luna::AssetClass* GetAssetClass() const NOC_OVERRIDE;
        virtual void CreateChildren();
        virtual void ActivateItem() NOC_OVERRIDE;
        void Load();
        void Unload();
        Nocturnal::Path GetAssetPath() const;
        void SetAssetPath( const Nocturnal::Path& path );
        void AssociateField( Reflect::Element* element, const Reflect::Field* field );
        bool IsFieldAssociated() const;
        virtual void ConnectProperties( EnumerateElementArgs& args ) NOC_OVERRIDE;
        bool ValidatePanel( const tstring& name ) NOC_OVERRIDE;
        virtual void PreShowContextMenu() NOC_OVERRIDE;
        virtual bool CanBeCopied() const NOC_OVERRIDE;
        virtual Inspect::ReflectClipboardDataPtr GetClipboardData() NOC_OVERRIDE;
        virtual bool CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data ) NOC_OVERRIDE;
        virtual bool HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch = NULL ) NOC_OVERRIDE;

        // Helpers
    private:
        Luna::ComponentNode* FindComponentNode( Luna::ComponentWrapper* attribute );
        Undo::CommandPtr GetAddComponentCommand( Luna::ComponentWrapper* attribute );
        Undo::CommandPtr GetRemoveComponentCommand( Luna::ComponentWrapper* attribute );
        friend class ComponentExistenceCommand;

        static void CreatePanel( CreatePanelArgs& args );
        tstring GetFileName() const;
        tstring GetFilePath() const;
        void SetFilePath( const tstring& path );
        tstring GetAssetTypeName() const;
        tstring MakeLabel() const;
        tstring MakeIcon() const;

        // Callbacks
    private:
        void AssetUnloading( const AssetLoadArgs& args );
        void PreferenceChanged( const Reflect::ElementChangeArgs& args );
        void DocumentModified( const DocumentChangedArgs& args );
        void OnElementChanged( const Reflect::ElementChangeArgs& args );
        void AssetTypeChanged( const Asset::AssetTypeChangeArgs& args );

        void OnOpen( const ContextMenuArgsPtr& args );
        void OnChangePath( const ContextMenuArgsPtr& args );
        void OnChangePathFinder( const ContextMenuArgsPtr& args );
        void OnCheckOutPath( const ContextMenuArgsPtr& args );
        void OnRevisionHistory( const ContextMenuArgsPtr& args );
    };

    typedef Nocturnal::SmartPtr< Luna::AssetReferenceNode > AssetReferenceNodePtr;
}
