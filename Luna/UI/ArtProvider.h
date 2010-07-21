#pragma once

#include "Application/UI/ArtProvider.h"

namespace Luna
{
    // Nocturnal's custom art IDs
    namespace ArtIDs
    {
        static const wxChar* NewScene = wxART_MAKE_ART_ID( LUNA_ART_ID_NEW_SCENE );
        static const wxChar* NewEntity = wxART_MAKE_ART_ID( LUNA_ART_ID_NEW_ENTITY );

        static const wxChar* PerspectiveCamera = wxART_MAKE_ART_ID( LUNA_ART_ID_PERSPECTIVE_CAMERA );
        static const wxChar* FrontOrthoCamera = wxART_MAKE_ART_ID( LUNA_ART_ID_FRONT_ORTHO_CAMERA );
        static const wxChar* SideOrthoCamera = wxART_MAKE_ART_ID( LUNA_ART_ID_SIDE_ORTHO_CAMERA );
        static const wxChar* TopOrthoCamera = wxART_MAKE_ART_ID( LUNA_ART_ID_TOP_ORTHO_CAMERA );

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
        static const wxChar* LunaIcon = wxART_MAKE_ART_ID( LUNA_ART_ID_LUNA_ICON );

    }

    class ArtProvider : public Nocturnal::ArtProvider
    {
    public:
        ArtProvider();

        void Create();
    };
}