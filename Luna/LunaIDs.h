#pragma once

#include "Application/UI/ArtProvider.h"

namespace Luna
{
    namespace LunaIDs
    {
        enum LunaID
        {
            // controls
            ID_HierarchyOutlineControl = wxID_HIGHEST+1,
            ID_TypeOutlineControl,
            ID_SelectionProperties,
            ID_ToolProperties,
            ID_Properties,
            ID_GridNotebook,
            ID_LayerGrid,
            ID_ObjectGrid,

            ID_NewScene,
            ID_NewEntity,

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
    typedef LunaIDs::LunaID LunaID;
}
