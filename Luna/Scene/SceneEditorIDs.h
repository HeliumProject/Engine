#pragma once

#include "Application/UI/ArtProvider.h"

namespace Luna
{
    namespace SceneEditorIDs
    {
        enum SceneEditorID
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
            ID_ViewPathfinding,

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
            ID_ToolsNavMesh,
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

            ID_ZoneNew,
            ID_ZoneOpen,
            ID_ZoneMoveSel,

            ID_About,

            // This needs to be last, since command entries will be dynamically generated from this id onward
            ID_SelectContextMenu,
        };
    }
    typedef SceneEditorIDs::SceneEditorID SceneEditorID;

    namespace ToolModes
    {
        enum ToolMode
        {
            Begin = SceneEditorIDs::ID_ToolsSelect,
            Select = SceneEditorIDs::ID_ToolsSelect,
            Scale = SceneEditorIDs::ID_ToolsScale,
            Rotate = SceneEditorIDs::ID_ToolsRotate,
            Translate = SceneEditorIDs::ID_ToolsTranslate,
            Duplicate = SceneEditorIDs::ID_ToolsDuplicate,
            LocatorCreate = SceneEditorIDs::ID_ToolsLocatorCreate,
            VolumeCreate = SceneEditorIDs::ID_ToolsVolumeCreate,
            EntityCreate = SceneEditorIDs::ID_ToolsEntityCreate,
            CurveCreate = SceneEditorIDs::ID_ToolsCurveCreate,
            CurveEdit = SceneEditorIDs::ID_ToolsCurveEdit,
            NavMesh = SceneEditorIDs::ID_ToolsNavMesh,
            End = SceneEditorIDs::ID_ToolsEnd,
            Count = SceneEditorIDs::ID_ToolsCount,
        };
    }

    struct ToolInfo
    {
        i32             m_ID;
        const tchar*    m_Name;
        const tchar*    m_Bitmap;
        const tchar*    m_Description;
    };

    static ToolInfo ToolInfos[] =
    {
        { ToolModes::Select,            wxT("Select"),          Nocturnal::ArtIDs::Select,      wxT("Select items from the workspace") },
        { ToolModes::Translate,         wxT("Translate"),       Nocturnal::ArtIDs::Translate,   wxT("Translate items") },
        { ToolModes::Rotate,            wxT("Rotate"),          Nocturnal::ArtIDs::Rotate,      wxT("Rotate selected items") },
        { ToolModes::Scale,             wxT("Scale"),           Nocturnal::ArtIDs::Scale,       wxT("Scale selected items") },
        { ToolModes::Duplicate,         wxT("Duplicate"),       Nocturnal::ArtIDs::Duplicate,   wxT("Duplicate the selected object numerous times") },
        { ToolModes::LocatorCreate,     wxT("Create Locator"),  Nocturnal::ArtIDs::Locator,     wxT("Place locator objects (such as bug locators)") },
        { ToolModes::VolumeCreate,      wxT("Create Volume"),   Nocturnal::ArtIDs::Volume,      wxT("Place volume objects (items for setting up gameplay)") },
        { ToolModes::EntityCreate,      wxT("Create Entity"),   Nocturnal::ArtIDs::Entity,      wxT("Place entity objects (such as art instances or characters)") },
        { ToolModes::CurveCreate,       wxT("Create Curve"),    Nocturnal::ArtIDs::Curve,       wxT("Create curve objects (Linear, B-Spline, or Catmull-Rom Spline)") },
        { ToolModes::CurveEdit,         wxT("Edit Curve"),      Nocturnal::ArtIDs::CurveEdit,   wxT("Edit created curves (modify or create/delete control points)") },
        { ToolModes::NavMesh,           wxT("Edit NavMesh"),    Nocturnal::ArtIDs::NavMesh,     wxT("Create NavMesh or add new verts and tris") },
    };

    NOC_COMPILE_ASSERT( sizeof(ToolInfos) == SceneEditorIDs::ID_ToolsCount * sizeof(ToolInfo) );
}
