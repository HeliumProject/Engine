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

            IconArtFile& AddOverlay( const tchar* filename, OverlayQuadrants::OverlayQuadrant quadrant = OverlayQuadrants::BottomRight );

            static void CalculatePlacement( wxImage &target_image, const wxImage &source_image, OverlayQuadrants::OverlayQuadrant quadrant, int &x, int &y );
            static void Paste( wxImage &target_image, const wxImage &source_image, int x, int y, bool blendAlpha = false );
        };

        ///////////////////////////////////////////////////////////////////////
        namespace ArtIDs
        {
            static const wxChar* Null = wxART_MAKE_ART_ID( HELIUM_ART_ID_NULL );

            namespace Actions
            {
                static const wxChar* Add = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_ADD );
                static const wxChar* Copy = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_COPY );
                static const wxChar* Create = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_CREATE );
                static const wxChar* Cut = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_CUT );
                static const wxChar* Delete = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_DELETE );
                static const wxChar* Down = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_DOWN );
                static const wxChar* Edit = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_EDIT );
                static const wxChar* FileAdd = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_FILE_ADD );
                static const wxChar* FileDelete = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_FILE_DELETE );
                static const wxChar* Find = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_FIND );
                static const wxChar* Next = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_NEXT );
                static const wxChar* Open = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_OPEN );
                static const wxChar* Options = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_OPTIONS );
                static const wxChar* Paste = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_PASTE );
                static const wxChar* Pause = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_PAUSE );
                static const wxChar* Play = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_PLAY );
                static const wxChar* Previous = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_PREVIOUS );
                static const wxChar* Redo = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_REDO );
                static const wxChar* Refresh = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_REFRESH );
                static const wxChar* Save = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SAVE );
                static const wxChar* SaveAll = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SAVE_ALL );
                static const wxChar* SaveAs = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SAVE_AS );
                static const wxChar* Select = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SELECT );
                static const wxChar* SelectAll = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SELECT_ALL );
                static const wxChar* Settings = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_SETTINGS );
                static const wxChar* Stop = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_STOP );
                static const wxChar* Undo = wxART_MAKE_ART_ID( HELIUM_ART_ID_ACTION_UNDO );
            }

            namespace Editor
            {
                static const wxChar* AddSelectionToLayers = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_ADD_SELECTION_TO_LAYERS );
                static const wxChar* BackfaceCulling = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_BACKFACE_CULL );
                static const wxChar* Camera = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CAMERA );
                static const wxChar* ColorModeLayer = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_COLOR_MODE_LAYER );
                static const wxChar* ColorModeNodeType = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_COLOR_MODE_NODE_TYPE );
                static const wxChar* ColorModeScale = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_COLOR_MODE_SCALE );
                static const wxChar* ColorModeScaleGradient = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_COLOR_MODE_SCALE_GRADIENT );
                static const wxChar* ColorModeScene = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_COLOR_MODE_SCENE );
                static const wxChar* CreateNewLayer = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CREATE_NEW_LAYER );
                static const wxChar* CreateNewLayerFromSelection = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CREATE_NEW_LAYER_FROM_SELECTION );
                static const wxChar* Curve = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CURVE );
                static const wxChar* CurveEdit = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CURVE_EDIT );
                static const wxChar* DeleteSelectedLayers = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_DELETE_SELECTED_LAYERS );
                static const wxChar* DuplicateTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_DUPLICATE_TOOL );
                static const wxChar* EditorIcon = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_EDITOR_ICON );
                static const wxChar* Entity = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_NEW_ENTITY );
                static const wxChar* EntityPlacementTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_ENTITY_PLACEMENT_TOOL );
                static const wxChar* FrameOrigin = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_FRAME_ORIGIN );
                static const wxChar* FrameSelected = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_FRAME_SELECTED );
                static const wxChar* FrontOrthoCamera = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_FRONT_ORTHO_CAMERA );
                static const wxChar* FrustumCulling = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_FRUSTUM_CULL );
                static const wxChar* Helium = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_HELIUM );
                static const wxChar* HighlightMode = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_HIGHLIGHT_MODE );
                static const wxChar* Locator = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_LOCATOR );
                static const wxChar* NextView = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_NEXT_VIEW );
                static const wxChar* PerspectiveCamera = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_PERSPECTIVE_CAMERA );
                static const wxChar* PreviousView = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_PREVIOUS_VIEW );
                static const wxChar* ProjectFolder = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_CREATE_PROJECT_FOLDER );
                static const wxChar* RemoveSelectionFromLayers = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_REMOVE_SELECTION_FROM_LAYERS );
                static const wxChar* RotateTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_ROTATE_TOOL );
                static const wxChar* ScaleTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SCALE_TOOL );
                static const wxChar* Scene = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_NEW_SCENE );
                static const wxChar* SelectLayerMembers = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SELECT_LAYER_MEMBERS );
                static const wxChar* SelectLayers = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SELECT_LAYERS );
                static const wxChar* SelectTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SELECT_TOOL );
                static const wxChar* ShadingMaterial = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHADING_MATERIAL );
                static const wxChar* ShadingWireframe = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHADING_WIREFRAME );
                static const wxChar* ShowAxes = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHOW_AXES );
                static const wxChar* ShowBounds = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHOW_BOUNDS );
                static const wxChar* ShowGrid = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHOW_GRID );
                static const wxChar* ShowStatistics = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SHOW_STATISTICS );
                static const wxChar* SideOrthoCamera = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_SIDE_ORTHO_CAMERA );
                static const wxChar* TaskWindow = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_TASKWINDOW );
                static const wxChar* TopOrthoCamera = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_TOP_ORTHO_CAMERA );
                static const wxChar* TranslateTool = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_TRANSLATE_TOOL );
                static const wxChar* Vault = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_VAULT );
                static const wxChar* Volume = wxART_MAKE_ART_ID( HELIUM_ART_ID_EDITOR_VOLUME );
            }

            namespace Devices
            {
            }

            namespace FileSystem
            {
                static const wxChar* File = wxART_MAKE_ART_ID( HELIUM_ART_ID_FILE );
                static const wxChar* Folder = wxART_MAKE_ART_ID( HELIUM_ART_ID_FOLDER );
            }

            namespace MimeTypes
            {
                static const wxChar* Binary = wxART_MAKE_ART_ID( HELIUM_ART_ID_BINARY );
                static const wxChar* ReflectBinary = wxART_MAKE_ART_ID( HELIUM_ART_ID_REFLECTBINARY );
                static const wxChar* Text = wxART_MAKE_ART_ID( HELIUM_ART_ID_TEXT );
            }

            namespace RevisionControl
            {
                static const wxChar* Active = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_ACTIVE );
                static const wxChar* Changelist = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHANGELIST );
                static const wxChar* CheckIn = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKIN );
                static const wxChar* CheckOut = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKOUT );
                static const wxChar* CheckedOutByMe = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKED_OUT_BY_ME );
                static const wxChar* CheckedOutBySomeoneElse = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKED_OUT_BY_SOMEONE_ELSE );
                static const wxChar* CreateChangelist = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CREATECHANGELIST );
                static const wxChar* File = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_FILE );
                static const wxChar* History = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_HISTORY );
                static const wxChar* Merge = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_MERGE );
                static const wxChar* OutOfDate = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_OUTOFDATE );
                static const wxChar* Revert = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_REVERT );
                static const wxChar* RevisionControlled = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CONTROLLED );
                static const wxChar* Submit = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_SUBMIT );
                static const wxChar* Unknown = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_UNKNOWN );
                static const wxChar* User = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_USER );
            }

            namespace Status
            {
                static const wxChar* Lockable = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_LOCKABLE );
                static const wxChar* Locked = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_LOCKED );
                static const wxChar* Busy = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_BUSY );
                static const wxChar* Information = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_INFORMATION );
                static const wxChar* Warning = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_WARNING );
                static const wxChar* Error = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_ERROR );
                static const wxChar* Unknown = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_UNKNOWN );
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