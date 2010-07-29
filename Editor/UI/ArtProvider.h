#pragma once

#include "Application/UI/ArtProvider.h"

namespace Editor
{
    // Helium's custom art IDs
    namespace ArtIDs
    {
        static const wxChar* NewScene = wxART_MAKE_ART_ID( LUNA_ART_ID_NEW_SCENE );
        static const wxChar* NewEntity = wxART_MAKE_ART_ID( LUNA_ART_ID_NEW_ENTITY );

        static const wxChar* FrameOrigin = wxART_MAKE_ART_ID( LUNA_ART_ID_FRAME_ORIGIN );
        static const wxChar* FrameSelected = wxART_MAKE_ART_ID( LUNA_ART_ID_FRAME_SELECTED );

        static const wxChar* PreviousView = wxART_MAKE_ART_ID( LUNA_ART_ID_PREVIOUS_VIEW );
        static const wxChar* NextView = wxART_MAKE_ART_ID( LUNA_ART_ID_NEXT_VIEW );

        static const wxChar* HighlightMode = wxART_MAKE_ART_ID( LUNA_ART_ID_HIGHLIGHT_MODE );

        static const wxChar* PerspectiveCamera = wxART_MAKE_ART_ID( LUNA_ART_ID_PERSPECTIVE_CAMERA );
        static const wxChar* FrontOrthoCamera = wxART_MAKE_ART_ID( LUNA_ART_ID_FRONT_ORTHO_CAMERA );
        static const wxChar* SideOrthoCamera = wxART_MAKE_ART_ID( LUNA_ART_ID_SIDE_ORTHO_CAMERA );
        static const wxChar* TopOrthoCamera = wxART_MAKE_ART_ID( LUNA_ART_ID_TOP_ORTHO_CAMERA );

        static const wxChar* ShowAxes = wxART_MAKE_ART_ID( LUNA_ART_ID_SHOW_AXES );
        static const wxChar* ShowGrid = wxART_MAKE_ART_ID( LUNA_ART_ID_SHOW_GRID );
        static const wxChar* ShowBounds = wxART_MAKE_ART_ID( LUNA_ART_ID_SHOW_BOUNDS );
        static const wxChar* ShowStatistics = wxART_MAKE_ART_ID( LUNA_ART_ID_SHOW_STATISTICS );

        static const wxChar* FrustumCull = wxART_MAKE_ART_ID( LUNA_ART_ID_FRUSTUM_CULL );
        static const wxChar* BackfaceCull = wxART_MAKE_ART_ID( LUNA_ART_ID_BACKFACE_CULL );

        static const wxChar* ShadingWireframe = wxART_MAKE_ART_ID( LUNA_ART_ID_SHADING_WIREFRAME );
        static const wxChar* ShadingMaterial = wxART_MAKE_ART_ID( LUNA_ART_ID_SHADING_MATERIAL );

        static const wxChar* ColorModeScene = wxART_MAKE_ART_ID( LUNA_ART_ID_COLOR_MODE_SCENE );
        static const wxChar* ColorModeLayer = wxART_MAKE_ART_ID( LUNA_ART_ID_COLOR_MODE_LAYER );
        static const wxChar* ColorModeNodeType = wxART_MAKE_ART_ID( LUNA_ART_ID_COLOR_MODE_NODE_TYPE );
        static const wxChar* ColorModeScale = wxART_MAKE_ART_ID( LUNA_ART_ID_COLOR_MODE_SCALE );
        static const wxChar* ColorModeScaleGradient = wxART_MAKE_ART_ID( LUNA_ART_ID_COLOR_MODE_SCALE_GRADIENT );

        static const wxChar* SelectTool = wxART_MAKE_ART_ID( LUNA_ART_ID_SELECT_TOOL );
        static const wxChar* TranslateTool = wxART_MAKE_ART_ID( LUNA_ART_ID_TRANSLATE_TOOL );
        static const wxChar* RotateTool = wxART_MAKE_ART_ID( LUNA_ART_ID_ROTATE_TOOL );
        static const wxChar* ScaleTool = wxART_MAKE_ART_ID( LUNA_ART_ID_SCALE_TOOL );
        static const wxChar* DuplicateTool = wxART_MAKE_ART_ID( LUNA_ART_ID_DUPLICATE_TOOL );

        static const wxChar* Locator = wxART_MAKE_ART_ID( LUNA_ART_ID_LOCATOR );
        static const wxChar* Volume = wxART_MAKE_ART_ID( LUNA_ART_ID_VOLUME );
        static const wxChar* Entity = wxART_MAKE_ART_ID( LUNA_ART_ID_ENTITY );
        static const wxChar* Curve = wxART_MAKE_ART_ID( LUNA_ART_ID_CURVE );
        static const wxChar* CurveEdit = wxART_MAKE_ART_ID( LUNA_ART_ID_CURVEEDIT );
        static const wxChar* NavMesh = wxART_MAKE_ART_ID( LUNA_ART_ID_NAVMESH );

        static const wxChar* CreateNewLayer = wxART_MAKE_ART_ID( LUNA_ART_ID_CREATE_NEW_LAYER );
        static const wxChar* CreateNewLayerFromSelection = wxART_MAKE_ART_ID( LUNA_ART_ID_CREATE_NEW_LAYER_FROM_SELECTION );
        static const wxChar* DeleteSelectedLayers = wxART_MAKE_ART_ID( LUNA_ART_ID_DELETE_SELECTED_LAYERS );
        static const wxChar* AddSelectionToLayers = wxART_MAKE_ART_ID( LUNA_ART_ID_ADD_SELECTION_TO_LAYERS );
        static const wxChar* RemoveSelectionFromLayers = wxART_MAKE_ART_ID( LUNA_ART_ID_REMOVE_SELECTION_FROM_LAYERS );
        static const wxChar* SelectLayerMembers = wxART_MAKE_ART_ID( LUNA_ART_ID_SELECT_LAYER_MEMBERS );
        static const wxChar* SelectLayers = wxART_MAKE_ART_ID( LUNA_ART_ID_SELECT_LAYERS );

        static const wxChar* NewCollection = wxART_MAKE_ART_ID( LUNA_ART_ID_NEW_COLLECTION );
        static const wxChar* DeleteCollection = wxART_MAKE_ART_ID( LUNA_ART_ID_DELETE_COLLECTION );
        
        static const wxChar* TaskWindow = wxART_MAKE_ART_ID( LUNA_ART_ID_TASKWINDOW );
        static const wxChar* EditorIcon = wxART_MAKE_ART_ID( LUNA_ART_ID_LUNA_ICON );

        static const wxChar* Play = wxART_MAKE_ART_ID( LUNA_ART_ID_PLAY );
        static const wxChar* Pause = wxART_MAKE_ART_ID( LUNA_ART_ID_PAUSE );
        static const wxChar* Stop = wxART_MAKE_ART_ID( LUNA_ART_ID_STOP );
    }

    class ArtProvider : public Helium::ArtProvider
    {
    public:
        ArtProvider();

        void Create();
    };
}