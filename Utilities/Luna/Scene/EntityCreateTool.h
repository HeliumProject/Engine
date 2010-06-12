#pragma once

#include "API.h"
#include "Entity.h"
#include "CreateTool.h"

#include "Inspect/FilteredDropTarget.h"

#include "Foundation/TUID.h"

namespace Luna
{
    class EntityRowInfo
    {
    public:
        std::string m_OriginalValue;
        std::string m_Name;
        Nocturnal::Path m_ClassPath;
        float m_Probability;

        std::string GetListName() const;
    };

    typedef std::vector< EntityRowInfo > V_EntityRowInfo;

    class EntityCreateTool : public Luna::CreateTool
    {
        //
        // Members
        //

    public:
        Nocturnal::Path m_ClassPath;
        static bool s_PointerVisible;
        static bool s_BoundsVisible;
        static bool s_GeometryVisible;

        //
        // RTTI
        //

        LUNA_DECLARE_TYPE(Luna::EntityCreateTool, Luna::CreateTool);
        static void InitializeType();
        static void CleanupType();

    protected:
        Inspect::List* m_RandomEntityList;
        V_EntityRowInfo m_RandomEntityInfo;
        static V_string s_RandomEntities;

        Inspect::FileDialogButton* m_FileButton;
        Inspect::FileBrowserButton* m_BrowserButton;

        Inspect::FileDialogButton* m_FileButtonAdd;
        Inspect::FileBrowserButton* m_BrowserButtonAdd;

    public:
        EntityCreateTool(Luna::Scene* scene, Enumerator* enumerator);
        virtual ~EntityCreateTool();

        virtual Luna::TransformPtr CreateNode() NOC_OVERRIDE;
        virtual void CreateProperties() NOC_OVERRIDE;

        std::string GetEntityAsset() const;
        void SetEntityAsset(const std::string& value);
        void AddEntityAsset(const std::string& value);

        bool GetPointerVisible() const;
        void SetPointerVisible(bool show);
        bool GetBoundsVisible() const;
        void SetBoundsVisible(bool show);
        bool GetGeometryVisible() const;
        void SetGeometryVisible(bool show);
        std::string GetRandomEntity() const;
        void SetRandomEntity( const std::string& entityName );

        void OnDeleteClass( Inspect::Button* button );
        void OnClear( Inspect::Button* button );
        void OnNormalize( Inspect::Button* button );
        void OnModify( Inspect::Button* button );

        void OnEntityDropped( const Inspect::FilteredDropTargetArgs& args );
        void DropEntities( const V_string& entities, bool appendToList );
    };
}
