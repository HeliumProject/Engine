#pragma once

#include "Editor/ArtProvider.h"

namespace Helium
{
    namespace Editor
    {
        namespace EventIds
        {
            enum EventId
            {
                // controls
                ID_ZonesControl = wxID_HIGHEST+1,
                ID_HierarchyOutlineControl,
                ID_TypeOutlineControl,
                ID_SelectionProperties,
                ID_ToolProperties,
                ID_Properties,
                ID_GridNotebook,
                ID_LayerGrid,
                ID_ObjectGrid,

                // menu items
                ID_FileCheckOut,
                ID_FileImport,
                ID_FileImportFromClipboard,
                ID_FileExport,
                ID_FileExportToClipboard,
                ID_FileExportToObj,
                ID_FileSaveAll,
                ID_FileSaveSession,
                ID_FileSaveSessionAs,
                ID_FileOpenSession,

                ID_EditParent,
                ID_EditUnparent,
                ID_EditGroup,
                ID_EditUngroup,
                ID_EditCenter,
                ID_EditInvertSelection,
                ID_EditSelectAll,
                ID_EditDuplicate,
                ID_EditSmartDuplicate,
                ID_EditCopyTransform,
                ID_EditPasteTransform,
                ID_EditSnapToCamera,
                ID_EditSnapCameraTo,

                ID_EditWalkUp,
                ID_EditWalkDown,
                ID_EditWalkForward,
                ID_EditWalkBackward,

                ID_EditPreferences,

                ID_ViewAxes,
                ID_ViewGrid,
                ID_ViewBounds,
                ID_ViewStatistics,

                ID_ViewOrbit, 
                ID_ViewFront,
                ID_ViewSide,
                ID_ViewTop,

                ID_ViewNone,
                ID_ViewRender,
                ID_ViewCollision,

                ID_ViewWireframeOnMesh,
                ID_ViewWireframeOnShaded,
                ID_ViewWireframe,
                ID_ViewMaterial, 
                ID_ViewTexture,

                ID_ViewFrustumCulling, 
                ID_ViewBackfaceCulling,

                ID_ViewShowAll,
                ID_ViewShowAllGeometry,
                ID_ViewShowSelected,
                ID_ViewShowSelectedGeometry,
                ID_ViewShowUnrelated,
                ID_ViewShowLastHidden,

                ID_ViewHideAll,
                ID_ViewHideAllGeometry,
                ID_ViewHideSelected,
                ID_ViewHideSelectedGeometry,
                ID_ViewHideUnrelated,

                ID_ViewFrameOrigin,
                ID_ViewFrameSelected,
                ID_ViewHighlightMode,

                ID_ViewPreviousView,
                ID_ViewNextView,

                ID_ViewDefaultShowLayers, 
                ID_ViewDefaultShowInstances, 
                ID_ViewDefaultShowGeometry, 
                ID_ViewDefaultShowPointer, 
                ID_ViewDefaultShowBounds,

                // start tools id region
                ID_ToolsBegin,
                ID_ToolsSelect = ID_ToolsBegin,
                ID_ToolsTranslate,
                ID_ToolsRotate,
                ID_ToolsScale,
                ID_ToolsDuplicate,
                ID_ToolsLocatorCreate,
                ID_ToolsVolumeCreate,
                ID_ToolsEntityCreate,
                ID_ToolsCurveCreate,
                ID_ToolsCurveEdit,
                ID_ToolsEnd,
                ID_ToolsCount = ID_ToolsEnd - ID_ToolsBegin,
                // end tools id region

                // these aren't tools, just menu items on the tools menu
                ID_ToolsScalePivot,
                ID_ToolsRotatePivot,
                ID_ToolsTranslatePivot,
                ID_ToolsPivot,

                ID_PanelsOutline,
                ID_PanelsProperties,

                ID_About,

                // This needs to be last, since command entries will be dynamically generated from this id onward
                ID_SelectContextMenu,
            };
        }
        typedef EventIds::EventId EventId;

        namespace ToolModes
        {
            enum ToolMode
            {
                Begin = EventIds::ID_ToolsSelect,
                Select = EventIds::ID_ToolsSelect,
                Scale = EventIds::ID_ToolsScale,
                Rotate = EventIds::ID_ToolsRotate,
                Translate = EventIds::ID_ToolsTranslate,
                Duplicate = EventIds::ID_ToolsDuplicate,
                LocatorCreate = EventIds::ID_ToolsLocatorCreate,
                VolumeCreate = EventIds::ID_ToolsVolumeCreate,
                EntityCreate = EventIds::ID_ToolsEntityCreate,
                CurveCreate = EventIds::ID_ToolsCurveCreate,
                CurveEdit = EventIds::ID_ToolsCurveEdit,
                End = EventIds::ID_ToolsEnd,
                Count = EventIds::ID_ToolsCount,
            };
        }

        namespace ToolTypes
        {
            enum ToolType
            {
                Manipulator,
                Creator,
                Editor,
            };
        }
        typedef ToolTypes::ToolType ToolType;

        struct ToolInfo
        {
            i32             m_ID;
            ToolType        m_Type;
            const tchar*    m_Name;
            const tchar*    m_Bitmap;
            const tchar*    m_Description;
        };

        static ToolInfo ToolInfos[] =
        {
            { ToolModes::Select,        ToolTypes::Manipulator,    wxT("Select"),          Editor::ArtIDs::SelectTool,    wxT("Select items from the workspace") },
            { ToolModes::Translate,     ToolTypes::Manipulator,    wxT("Translate"),       Editor::ArtIDs::TranslateTool, wxT("Translate items") },
            { ToolModes::Rotate,        ToolTypes::Manipulator,    wxT("Rotate"),          Editor::ArtIDs::RotateTool,    wxT("Rotate selected items") },
            { ToolModes::Scale,         ToolTypes::Manipulator,    wxT("Scale"),           Editor::ArtIDs::ScaleTool,     wxT("Scale selected items") },
            { ToolModes::Duplicate,     ToolTypes::Creator,        wxT("Duplicate"),       Editor::ArtIDs::DuplicateTool, wxT("Duplicate the selected object numerous times") },
            { ToolModes::LocatorCreate, ToolTypes::Creator,        wxT("Create Locator"),  Editor::ArtIDs::Locator,       wxT("Place locator objects (such as bug locators)") },
            { ToolModes::VolumeCreate,  ToolTypes::Creator,        wxT("Create Volume"),   Editor::ArtIDs::Volume,        wxT("Place volume objects (items for setting up gameplay)") },
            { ToolModes::EntityCreate,  ToolTypes::Creator,        wxT("Create Entity"),   Editor::ArtIDs::Entity,        wxT("Place entity objects (such as art instances or characters)") },
            { ToolModes::CurveCreate,   ToolTypes::Creator,        wxT("Create Curve"),    Editor::ArtIDs::Curve,         wxT("Create curve objects (Linear, B-Spline, or Catmull-Rom Spline)") },
#pragma TODO( "Add an overlay to the curve tool for editing" )
            { ToolModes::CurveEdit,     ToolTypes::Editor,         wxT("Edit Curve"),      Editor::ArtIDs::Curve,         wxT("Edit created curves (modify or create/delete control points)") },
        };

        HELIUM_COMPILE_ASSERT( sizeof(ToolInfos) == EventIds::ID_ToolsCount * sizeof(ToolInfo) );
    }
}