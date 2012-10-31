#pragma once

#include "Editor/API.h"

#include <map>

#include <wx/animate.h>
#include <wx/artprov.h>
#include <wx/imaglist.h>

#include "FileIconsTable.h"
#include "Foundation/FilePath.h"
#include "Platform/Types.h"

namespace Helium
{
    namespace Editor
    {

        ///////////////////////////////////////////////////////////////////////
        namespace OverlayQuadrants
        {
            enum OverlayQuadrant
            {
                TopLeft     = 0,
                TopCenter,
                TopRight,
                CenterLeft,
                Centered,
                CenterRight,
                BottomLeft,
                BottomCenter,
                BottomRight,
            };
        }
        typedef OverlayQuadrants::OverlayQuadrant OverlayQuadrant;
        typedef std::map< OverlayQuadrant, tstring > M_OverlayQuadrants;

        ///////////////////////////////////////////////////////////////////////
        class IconArtFile
        {
        public:
            tstring m_Filename;
            M_OverlayQuadrants m_Overlays;

        public:
            IconArtFile( const tstring& filename );

            IconArtFile& AddOverlay( const tchar_t* filename, OverlayQuadrants::OverlayQuadrant quadrant = OverlayQuadrants::BottomRight );

            static void CalculatePlacement( wxImage &target_image, const wxImage &source_image, OverlayQuadrants::OverlayQuadrant quadrant, int &x, int &y );
        };

        ///////////////////////////////////////////////////////////////////////
        namespace ArtIDs
        {
            static const char* Null = wxART_MAKE_ART_ID( HELIUM_ART_ID_NULL );

            namespace Actions
            {
                static const char* Add = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_ADD );
                static const char* Copy = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_COPY );
                static const char* Create = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_CREATE );
                static const char* Cut = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_CUT );
                static const char* Delete = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_DELETE );
                static const char* Down = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_DOWN );
                static const char* Edit = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_EDIT );
                static const char* FileAdd = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_FILE_ADD );
                static const char* FileDelete = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_FILE_DELETE );
                static const char* Find = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_FIND );
                static const char* Next = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_NEXT );
                static const char* Open = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_OPEN );
                static const char* Options = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_OPTIONS );
                static const char* Paste = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_PASTE );
                static const char* Pause = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_PAUSE );
                static const char* Play = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_PLAY );
                static const char* Previous = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_PREVIOUS );
                static const char* Redo = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_REDO );
                static const char* Refresh = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_REFRESH );
                static const char* Save = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SAVE );
                static const char* SaveAll = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SAVE_ALL );
                static const char* SaveAs = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SAVE_AS );
                static const char* Select = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SELECT );
                static const char* SelectAll = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SELECT_ALL );
                static const char* Settings = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SETTINGS );
                static const char* Stop = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_STOP );
                static const char* Undo = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_UNDO );
            }

            namespace Editor
            {
                static const char* AddSelectionToLayers = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_ADD_SELECTION_TO_LAYERS );
                static const char* BackfaceCulling = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_BACKFACE_CULL );
                static const char* Camera = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CAMERA );
                static const char* ColorModeLayer = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_COLOR_MODE_LAYER );
                static const char* ColorModeNodeType = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_COLOR_MODE_NODE_TYPE );
                static const char* ColorModeScale = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_COLOR_MODE_SCALE );
                static const char* ColorModeScaleGradient = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_COLOR_MODE_SCALE_GRADIENT );
                static const char* ColorModeScene = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_COLOR_MODE_SCENE );
                static const char* CreateNewLayer = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CREATE_NEW_LAYER );
                static const char* CreateNewLayerFromSelection = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CREATE_NEW_LAYER_FROM_SELECTION );
                static const char* Curve = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CURVE );
                static const char* CurveEdit = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CURVE_EDIT );
                static const char* DeleteSelectedLayers = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_DELETE_SELECTED_LAYERS );
                static const char* DuplicateTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_DUPLICATE_TOOL );
                static const char* EditorIcon = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_EDITOR_ICON );
                static const char* Entity = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_NEW_ENTITY );
                static const char* EntityPlacementTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_ENTITY_PLACEMENT_TOOL );
                static const char* FrameOrigin = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_FRAME_ORIGIN );
                static const char* FrameSelected = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_FRAME_SELECTED );
                static const char* FrontOrthoCamera = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_FRONT_ORTHO_CAMERA );
                static const char* FrustumCulling = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_FRUSTUM_CULL );
                static const char* Helium = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_HELIUM );
                static const char* HighlightMode = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_HIGHLIGHT_MODE );
                static const char* Locator = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_LOCATOR );
                static const char* NewProject = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_NEW_PROJECT );
                static const char* NextView = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_NEXT_VIEW );
                static const char* PerspectiveCamera = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_PERSPECTIVE_CAMERA );
                static const char* PreviousView = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_PREVIOUS_VIEW );
                static const char* ProjectFile = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_PROJECT_FILE );
                static const char* ProjectFolder = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_PROJECT_FOLDER );
                static const char* RemoveSelectionFromLayers = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_REMOVE_SELECTION_FROM_LAYERS );
                static const char* RotateTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_ROTATE_TOOL );
                static const char* ScaleTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SCALE_TOOL );
                static const char* Scene = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_NEW_SCENE );
                static const char* SelectLayerMembers = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SELECT_LAYER_MEMBERS );
                static const char* SelectLayers = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SELECT_LAYERS );
                static const char* SelectTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SELECT_TOOL );
                static const char* ShadingMaterial = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHADING_MATERIAL );
                static const char* ShadingWireframe = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHADING_WIREFRAME );
                static const char* ShadingTexture = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHADING_TEXTURE );
                static const char* ShowAxes = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHOW_AXES );
                static const char* ShowBounds = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHOW_BOUNDS );
                static const char* ShowGrid = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHOW_GRID );
                static const char* ShowStatistics = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHOW_STATISTICS );
                static const char* SideOrthoCamera = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SIDE_ORTHO_CAMERA );
                static const char* TaskWindow = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_TASKWINDOW );
                static const char* TopOrthoCamera = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_TOP_ORTHO_CAMERA );
                static const char* TranslateTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_TRANSLATE_TOOL );
                static const char* Vault = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_VAULT );
                static const char* Volume = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_VOLUME );
            }

            namespace Devices
            {
            }

            namespace FileSystem
            {
                static const char* File = wxART_MAKE_ART_ID( HELIUM_ART_ID_FILE );
                static const char* FindFolder = wxART_MAKE_ART_ID( HELIUM_ART_ID_FIND_FOLDER );
                static const char* Folder = wxART_MAKE_ART_ID( HELIUM_ART_ID_FOLDER );
            }

            namespace MimeTypes
            {
                static const char* Binary = wxART_MAKE_ART_ID( HELIUM_ART_ID_BINARY );
                static const char* Entity = wxART_MAKE_ART_ID( HELIUM_ART_ID_ENTITY );
                static const char* Project = wxART_MAKE_ART_ID( HELIUM_ART_ID_PROJECT );
                static const char* ReflectBinary = wxART_MAKE_ART_ID( HELIUM_ART_ID_REFLECTBINARY );
                static const char* Scene = wxART_MAKE_ART_ID( HELIUM_ART_ID_SCENE );
                static const char* Text = wxART_MAKE_ART_ID( HELIUM_ART_ID_TEXT );
            }

            namespace RevisionControl
            {
                static const char* Active = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_ACTIVE );
                static const char* Changelist = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHANGELIST );
                static const char* CheckIn = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKIN );
                static const char* CheckOut = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKOUT );
                static const char* CheckedOutByMe = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKED_OUT_BY_ME );
                static const char* CheckedOutBySomeoneElse = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKED_OUT_BY_SOMEONE_ELSE );
                static const char* CreateChangelist = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CREATECHANGELIST );
                static const char* File = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_FILE );
                static const char* History = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_HISTORY );
                static const char* Merge = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_MERGE );
                static const char* OutOfDate = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_OUTOFDATE );
                static const char* Revert = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_REVERT );
                static const char* RevisionControlled = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CONTROLLED );
                static const char* Submit = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_SUBMIT );
                static const char* Unknown = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_UNKNOWN );
                static const char* User = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_USER );
            }

            namespace Status
            {
                static const char* Lockable = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_LOCKABLE );
                static const char* Locked = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_LOCKED );
                static const char* Busy = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_BUSY );
                static const char* Information = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_INFORMATION );
                static const char* Warning = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_WARNING );
                static const char* Error = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_ERROR );
                static const char* Unknown = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_UNKNOWN );
            }

        }

        ///////////////////////////////////////////////////////////////////////
        class ArtProviderCache;

        class ArtProvider : public wxArtProvider
        {
        public:
            static wxSize DefaultIconSize;

            ArtProvider();
            virtual ~ArtProvider();

        protected:
            virtual wxBitmap CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size ) HELIUM_OVERRIDE;

        protected:
            // delayed initialization
            virtual void Create();

        protected:
            typedef std::map< wxArtID, IconArtFile > M_ArtIDToFilename;
            M_ArtIDToFilename m_ArtIDToFilename;

            ArtProviderCache *m_ArtProviderCache;    
        };
    }
}