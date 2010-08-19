#pragma once

#include "Editor/API.h"

#include <map>

#include <wx/animate.h>
#include <wx/artprov.h>
#include <wx/imaglist.h>

#include "FileIconsTable.h"
#include "Foundation/File/Path.h"
#include "Platform/Compiler.h"
#include "Platform/Types.h"

namespace Helium
{
    namespace Editor
    {
        namespace ArtIDs
        {
            static const wxChar* Unknown = wxART_MAKE_ART_ID( HELIUM_ART_ID_UNKNOWN );
            static const wxChar* Null = wxART_MAKE_ART_ID( HELIUM_ART_ID_NULL );

            static const wxChar* File = wxART_MAKE_ART_ID( HELIUM_ART_ID_FILE );
            static const wxChar* Folder = wxART_MAKE_ART_ID( HELIUM_ART_ID_FOLDER );

            namespace Verbs
            {
                static const wxChar* Create = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_CREATE );
                static const wxChar* Add = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_ADD );
                static const wxChar* Delete = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_DELETE );
                static const wxChar* Select = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_SELECT );
                static const wxChar* Refresh = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_REFRESH );
                static const wxChar* Find = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_FIND );
            }

            namespace Statuses
            {
                static const wxChar* Lockable = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_LOCKABLE );
                static const wxChar* Locked = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_LOCKED );
                static const wxChar* Busy = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_BUSY );
                static const wxChar* Information = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_INFORMATION );
                static const wxChar* Warning = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_WARNING );
                static const wxChar* Error = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_ERROR );
                static const wxChar* Binary = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_BINARY );
                static const wxChar* Text = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_TEXT );
            }

            static const wxChar* Open = wxART_MAKE_ART_ID( HELIUM_ART_ID_OPEN );
            static const wxChar* Save = wxART_MAKE_ART_ID( HELIUM_ART_ID_SAVE );
            static const wxChar* SaveAs = wxART_MAKE_ART_ID( HELIUM_ART_ID_SAVE_AS );
            static const wxChar* SaveAll = wxART_MAKE_ART_ID( HELIUM_ART_ID_SAVE_ALL );

            static const wxChar* SelectAll = wxART_MAKE_ART_ID( HELIUM_ART_ID_SELECT_ALL );

            static const wxChar* Cut = wxART_MAKE_ART_ID( HELIUM_ART_ID_CUT );
            static const wxChar* Copy = wxART_MAKE_ART_ID( HELIUM_ART_ID_COPY );
            static const wxChar* Paste = wxART_MAKE_ART_ID( HELIUM_ART_ID_PASTE );

            static const wxChar* Create = wxART_MAKE_ART_ID( HELIUM_ART_ID_CREATE );
            static const wxChar* Add = wxART_MAKE_ART_ID( HELIUM_ART_ID_ADD );
            static const wxChar* Delete = wxART_MAKE_ART_ID( HELIUM_ART_ID_DELETE );

            static const wxChar* Undo = wxART_MAKE_ART_ID( HELIUM_ART_ID_UNDO );
            static const wxChar* Redo = wxART_MAKE_ART_ID( HELIUM_ART_ID_REDO );

            static const wxChar* Options = wxART_MAKE_ART_ID( HELIUM_ART_ID_OPTIONS );
            static const wxChar* Settings = wxART_MAKE_ART_ID( HELIUM_ART_ID_SETTINGS );

            static const wxChar* Find = wxART_MAKE_ART_ID( HELIUM_ART_ID_FIND );

            namespace RevisionControl
            {
                static const wxChar* Unknown = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_UNKNOWN );
                static const wxChar* Active = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_ACTIVE );
                static const wxChar* CheckedOutByMe = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKED_OUT_BY_ME );
                static const wxChar* CheckedOutBySomeoneElse = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKED_OUT_BY_SOMEONE_ELSE );
                static const wxChar* OutOfDate = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_OUTOFDATE );
                static const wxChar* CheckOut = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKOUT );
                static const wxChar* CheckIn = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKIN );
                static const wxChar* Submit = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_SUBMIT );
                static const wxChar* Revert = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_REVERT );
                static const wxChar* Merge = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_MERGE );
                static const wxChar* History = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_HISTORY );
                static const wxChar* File = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_FILE );
                static const wxChar* Changelist = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHANGELIST );
                static const wxChar* CreateChangelist = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CREATECHANGELIST );
                static const wxChar* User = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_USER );

                static const wxChar* RevisionControlled = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CONTROLLED );
            }

            static const wxChar* Scene = wxART_MAKE_ART_ID( EDITOR_ART_ID_NEW_SCENE );
            static const wxChar* Entity = wxART_MAKE_ART_ID( EDITOR_ART_ID_NEW_ENTITY );

            static const wxChar* FrameOrigin = wxART_MAKE_ART_ID( EDITOR_ART_ID_FRAME_ORIGIN );
            static const wxChar* FrameSelected = wxART_MAKE_ART_ID( EDITOR_ART_ID_FRAME_SELECTED );

            static const wxChar* PreviousView = wxART_MAKE_ART_ID( EDITOR_ART_ID_PREVIOUS_VIEW );
            static const wxChar* NextView = wxART_MAKE_ART_ID( EDITOR_ART_ID_NEXT_VIEW );

            static const wxChar* HighlightMode = wxART_MAKE_ART_ID( EDITOR_ART_ID_HIGHLIGHT_MODE );

            static const wxChar* PerspectiveCamera = wxART_MAKE_ART_ID( EDITOR_ART_ID_PERSPECTIVE_CAMERA );
            static const wxChar* FrontOrthoCamera = wxART_MAKE_ART_ID( EDITOR_ART_ID_FRONT_ORTHO_CAMERA );
            static const wxChar* SideOrthoCamera = wxART_MAKE_ART_ID( EDITOR_ART_ID_SIDE_ORTHO_CAMERA );
            static const wxChar* TopOrthoCamera = wxART_MAKE_ART_ID( EDITOR_ART_ID_TOP_ORTHO_CAMERA );

            static const wxChar* ShowAxes = wxART_MAKE_ART_ID( EDITOR_ART_ID_SHOW_AXES );
            static const wxChar* ShowGrid = wxART_MAKE_ART_ID( EDITOR_ART_ID_SHOW_GRID );
            static const wxChar* ShowBounds = wxART_MAKE_ART_ID( EDITOR_ART_ID_SHOW_BOUNDS );
            static const wxChar* ShowStatistics = wxART_MAKE_ART_ID( EDITOR_ART_ID_SHOW_STATISTICS );

            static const wxChar* FrustumCulling = wxART_MAKE_ART_ID( EDITOR_ART_ID_FRUSTUM_CULL );
            static const wxChar* BackfaceCulling = wxART_MAKE_ART_ID( EDITOR_ART_ID_BACKFACE_CULL );

            static const wxChar* ShadingWireframe = wxART_MAKE_ART_ID( EDITOR_ART_ID_SHADING_WIREFRAME );
            static const wxChar* ShadingMaterial = wxART_MAKE_ART_ID( EDITOR_ART_ID_SHADING_MATERIAL );

            static const wxChar* ColorModeScene = wxART_MAKE_ART_ID( EDITOR_ART_ID_COLOR_MODE_SCENE );
            static const wxChar* ColorModeLayer = wxART_MAKE_ART_ID( EDITOR_ART_ID_COLOR_MODE_LAYER );
            static const wxChar* ColorModeNodeType = wxART_MAKE_ART_ID( EDITOR_ART_ID_COLOR_MODE_NODE_TYPE );
            static const wxChar* ColorModeScale = wxART_MAKE_ART_ID( EDITOR_ART_ID_COLOR_MODE_SCALE );
            static const wxChar* ColorModeScaleGradient = wxART_MAKE_ART_ID( EDITOR_ART_ID_COLOR_MODE_SCALE_GRADIENT );

            static const wxChar* SelectTool = wxART_MAKE_ART_ID( EDITOR_ART_ID_SELECT_TOOL );
            static const wxChar* TranslateTool = wxART_MAKE_ART_ID( EDITOR_ART_ID_TRANSLATE_TOOL );
            static const wxChar* RotateTool = wxART_MAKE_ART_ID( EDITOR_ART_ID_ROTATE_TOOL );
            static const wxChar* ScaleTool = wxART_MAKE_ART_ID( EDITOR_ART_ID_SCALE_TOOL );
            static const wxChar* DuplicateTool = wxART_MAKE_ART_ID( EDITOR_ART_ID_DUPLICATE_TOOL );

            static const wxChar* Camera = wxART_MAKE_ART_ID( EDITOR_ART_ID_CAMERA );
            static const wxChar* Locator = wxART_MAKE_ART_ID( EDITOR_ART_ID_LOCATOR );
            static const wxChar* Volume = wxART_MAKE_ART_ID( EDITOR_ART_ID_VOLUME );
            static const wxChar* EntityPlacementTool = wxART_MAKE_ART_ID( EDITOR_ART_ID_ENTITY_PLACEMENT_TOOL );
            static const wxChar* Curve = wxART_MAKE_ART_ID( EDITOR_ART_ID_CURVE );

            static const wxChar* ProjectFolder = wxART_MAKE_ART_ID( EDITOR_ART_ID_CREATE_PROJECT_FOLDER );

            static const wxChar* CreateNewLayer = wxART_MAKE_ART_ID( EDITOR_ART_ID_CREATE_NEW_LAYER );
            static const wxChar* CreateNewLayerFromSelection = wxART_MAKE_ART_ID( EDITOR_ART_ID_CREATE_NEW_LAYER_FROM_SELECTION );
            static const wxChar* DeleteSelectedLayers = wxART_MAKE_ART_ID( EDITOR_ART_ID_DELETE_SELECTED_LAYERS );
            static const wxChar* AddSelectionToLayers = wxART_MAKE_ART_ID( EDITOR_ART_ID_ADD_SELECTION_TO_LAYERS );
            static const wxChar* RemoveSelectionFromLayers = wxART_MAKE_ART_ID( EDITOR_ART_ID_REMOVE_SELECTION_FROM_LAYERS );
            static const wxChar* SelectLayerMembers = wxART_MAKE_ART_ID( EDITOR_ART_ID_SELECT_LAYER_MEMBERS );
            static const wxChar* SelectLayers = wxART_MAKE_ART_ID( EDITOR_ART_ID_SELECT_LAYERS );

            static const wxChar* Collection = wxART_MAKE_ART_ID( EDITOR_ART_ID_NEW_COLLECTION );

            static const wxChar* TaskWindow = wxART_MAKE_ART_ID( EDITOR_ART_ID_TASKWINDOW );
            static const wxChar* EditorIcon = wxART_MAKE_ART_ID( EDITOR_ART_ID_EDITOR_ICON );

            static const wxChar* Play = wxART_MAKE_ART_ID( EDITOR_ART_ID_PLAY );
            static const wxChar* Pause = wxART_MAKE_ART_ID( EDITOR_ART_ID_PAUSE );
            static const wxChar* Stop = wxART_MAKE_ART_ID( EDITOR_ART_ID_STOP );

            static const wxChar* Vault = wxART_MAKE_ART_ID( EDITOR_ART_ID_VAULT );
        }

        /////////////////////////////////////////////////////////////////////////////
        class ArtProviderCache;

        class ArtProvider : public wxArtProvider
        {
        public:
            static wxSize DefaultImageSize;

            ArtProvider();
            virtual ~ArtProvider();

        protected:
            virtual wxBitmap CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size ) HELIUM_OVERRIDE;

        protected:
            // delayed initialization
            virtual void Create();

        protected:
            typedef std::map< wxArtID, tstring > M_ArtIDToFilename;
            M_ArtIDToFilename m_ArtIDToFilename;

            ArtProviderCache *m_ArtProviderCache;    
        };
    }
}