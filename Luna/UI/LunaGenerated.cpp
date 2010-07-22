///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"


#include "LunaGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

MainFrameGenerated::MainFrameGenerated( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : Luna::Frame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_MainMenuBar = new wxMenuBar( 0 );
	m_MenuFile = new wxMenu();
	m_MenuFileNew = new wxMenu();
	wxMenuItem* m_ItemFileNewScene;
	m_ItemFileNewScene = new wxMenuItem( m_MenuFileNew, ID_NewScene, wxString( _("Scene...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFileNew->Append( m_ItemFileNewScene );
	
	wxMenuItem* m_ItemFileNewEntity;
	m_ItemFileNewEntity = new wxMenuItem( m_MenuFileNew, ID_NewEntity, wxString( _("Entity...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFileNew->Append( m_ItemFileNewEntity );
	
	wxMenuItem* m_ItemFileNewProject;
	m_ItemFileNewProject = new wxMenuItem( m_MenuFileNew, ID_NewProject, wxString( _("Project...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFileNew->Append( m_ItemFileNewProject );
	
	m_MenuFile->Append( -1, _("New"), m_MenuFileNew );
	
	wxMenuItem* m_ItemOpen;
	m_ItemOpen = new wxMenuItem( m_MenuFile, ID_Open, wxString( _("Open...") ) + wxT('\t') + wxT("ctrl-o"), wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemOpen );
	
	m_MenuFileOpenRecent = new wxMenu();
	m_MenuFile->Append( -1, _("Open Recent"), m_MenuFileOpenRecent );
	
	wxMenuItem* m_ItemClose;
	m_ItemClose = new wxMenuItem( m_MenuFile, ID_Close, wxString( _("Close") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemClose );
	
	wxMenuItem* m_separator1;
	m_separator1 = m_MenuFile->AppendSeparator();
	
	wxMenuItem* m_ItemSaveAll;
	m_ItemSaveAll = new wxMenuItem( m_MenuFile, ID_SaveAll, wxString( _("Save All") ) + wxT('\t') + wxT("ctrl-s"), wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemSaveAll );
	
	wxMenuItem* m_separator2;
	m_separator2 = m_MenuFile->AppendSeparator();
	
	wxMenuItem* m_ItemImport;
	m_ItemImport = new wxMenuItem( m_MenuFile, ID_Import, wxString( _("Import...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemImport );
	
	wxMenuItem* m_ItemImportFromClipboard;
	m_ItemImportFromClipboard = new wxMenuItem( m_MenuFile, ID_ImportFromClipboard, wxString( _("Import from Clipboard...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemImportFromClipboard );
	
	wxMenuItem* m_ItemExport;
	m_ItemExport = new wxMenuItem( m_MenuFile, ID_Export, wxString( _("Export...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemExport );
	
	wxMenuItem* m_ItemExportToClipboard;
	m_ItemExportToClipboard = new wxMenuItem( m_MenuFile, ID_ExportToClipboard, wxString( _("Export to Clipboard...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemExportToClipboard );
	
	wxMenuItem* m_separator4;
	m_separator4 = m_MenuFile->AppendSeparator();
	
	wxMenuItem* m_ItemExit;
	m_ItemExit = new wxMenuItem( m_MenuFile, ID_Exit, wxString( _("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemExit );
	
	m_MainMenuBar->Append( m_MenuFile, _("File") ); 
	
	m_MenuEdit = new wxMenu();
	wxMenuItem* m_ItemUndo;
	m_ItemUndo = new wxMenuItem( m_MenuEdit, wxID_UNDO, wxString( _("Undo") ) + wxT('\t') + wxT("ctrl-z"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemUndo );
	
	wxMenuItem* m_ItemRedo;
	m_ItemRedo = new wxMenuItem( m_MenuEdit, wxID_REDO, wxString( _("Redo") ) + wxT('\t') + wxT("ctrl-shift-z"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemRedo );
	
	wxMenuItem* m_separator5;
	m_separator5 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemCut;
	m_ItemCut = new wxMenuItem( m_MenuEdit, wxID_CUT, wxString( _("Cut") ) + wxT('\t') + wxT("ctrl-x"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemCut );
	
	wxMenuItem* m_ItemCopy;
	m_ItemCopy = new wxMenuItem( m_MenuEdit, wxID_COPY, wxString( _("Copy") ) + wxT('\t') + wxT("ctrl-c"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemCopy );
	
	wxMenuItem* m_ItemPaste;
	m_ItemPaste = new wxMenuItem( m_MenuEdit, wxID_PASTE, wxString( _("Paste") ) + wxT('\t') + wxT("ctrl-v"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemPaste );
	
	wxMenuItem* m_separator6;
	m_separator6 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemSelectAll;
	m_ItemSelectAll = new wxMenuItem( m_MenuEdit, ID_SelectAll, wxString( _("Select All") ) + wxT('\t') + wxT("ctrl-a"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemSelectAll );
	
	wxMenuItem* m_ItemInvertSelection;
	m_ItemInvertSelection = new wxMenuItem( m_MenuEdit, ID_InvertSelection, wxString( _("Invert Selection") ) + wxT('\t') + wxT("ctrl-i"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemInvertSelection );
	
	wxMenuItem* m_separator7;
	m_separator7 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemDelete;
	m_ItemDelete = new wxMenuItem( m_MenuEdit, wxID_DELETE, wxString( _("Delete") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemDelete );
	
	wxMenuItem* m_separator8;
	m_separator8 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemParent;
	m_ItemParent = new wxMenuItem( m_MenuEdit, ID_Parent, wxString( _("Parent") ) + wxT('\t') + wxT("ctrl-p"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemParent );
	
	wxMenuItem* m_ItemUnparent;
	m_ItemUnparent = new wxMenuItem( m_MenuEdit, ID_Unparent, wxString( _("Unparent") ) + wxT('\t') + wxT("ctrl-shift-p"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemUnparent );
	
	wxMenuItem* m_ItemGroup;
	m_ItemGroup = new wxMenuItem( m_MenuEdit, ID_Group, wxString( _("Group") ) + wxT('\t') + wxT("ctrl-g"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemGroup );
	
	wxMenuItem* m_ItemUngroup;
	m_ItemUngroup = new wxMenuItem( m_MenuEdit, ID_Ungroup, wxString( _("Ungroup") ) + wxT('\t') + wxT("ctrl-shift-g"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemUngroup );
	
	wxMenuItem* m_ItemCenter;
	m_ItemCenter = new wxMenuItem( m_MenuEdit, ID_Center, wxString( _("Center") ) + wxT('\t') + wxT("ctrl-shift-c"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemCenter );
	
	wxMenuItem* m_separator9;
	m_separator9 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemDuplicate;
	m_ItemDuplicate = new wxMenuItem( m_MenuEdit, ID_Duplicate, wxString( _("Duplicate") ) + wxT('\t') + wxT("ctrl-d"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemDuplicate );
	
	wxMenuItem* m_ItemSmartDuplicate;
	m_ItemSmartDuplicate = new wxMenuItem( m_MenuEdit, ID_SmartDuplicate, wxString( _("Smart Duplicate") ) + wxT('\t') + wxT("ctrl-shift-d"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemSmartDuplicate );
	
	wxMenuItem* m_separator10;
	m_separator10 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemCopyTransform;
	m_ItemCopyTransform = new wxMenuItem( m_MenuEdit, ID_CopyTransform, wxString( _("Copy Transform") ) + wxT('\t') + wxT("alt-t"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemCopyTransform );
	
	wxMenuItem* m_ItemPasteTransform;
	m_ItemPasteTransform = new wxMenuItem( m_MenuEdit, ID_PasteTransform, wxString( _("Paste Transform") ) + wxT('\t') + wxT("alt-shift-t"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemPasteTransform );
	
	wxMenuItem* m_separator11;
	m_separator11 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemSnapToCamera;
	m_ItemSnapToCamera = new wxMenuItem( m_MenuEdit, ID_SnapToCamera, wxString( _("Snap To Camera") ) + wxT('\t') + wxT("alt-c"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemSnapToCamera );
	
	wxMenuItem* m_ItemSnapCameraTo;
	m_ItemSnapCameraTo = new wxMenuItem( m_MenuEdit, ID_SnapCameraTo, wxString( _("Snap Camera To") ) + wxT('\t') + wxT("alt-shift-c"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemSnapCameraTo );
	
	wxMenuItem* m_separator12;
	m_separator12 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemWalkUp;
	m_ItemWalkUp = new wxMenuItem( m_MenuEdit, ID_WalkUp, wxString( _("Walk Up") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemWalkUp );
	
	wxMenuItem* m_ItemWalkDown;
	m_ItemWalkDown = new wxMenuItem( m_MenuEdit, ID_WalkDown, wxString( _("Walk Down") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemWalkDown );
	
	wxMenuItem* m_ItemWalkForward;
	m_ItemWalkForward = new wxMenuItem( m_MenuEdit, ID_WalkForward, wxString( _("Walk Forward") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemWalkForward );
	
	wxMenuItem* m_ItemWalkBackward;
	m_ItemWalkBackward = new wxMenuItem( m_MenuEdit, ID_WalkBackward, wxString( _("Walk Backward") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemWalkBackward );
	
	m_MainMenuBar->Append( m_MenuEdit, _("Edit") ); 
	
	m_MenuView = new wxMenu();
	m_MenuViewHelper = new wxMenu();
	wxMenuItem* m_ItemAxes;
	m_ItemAxes = new wxMenuItem( m_MenuViewHelper, wxID_ANY, wxString( _("Axes") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewHelper->Append( m_ItemAxes );
	
	wxMenuItem* m_ItemGrid;
	m_ItemGrid = new wxMenuItem( m_MenuViewHelper, wxID_ANY, wxString( _("Grid") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewHelper->Append( m_ItemGrid );
	
	wxMenuItem* m_ItemBounds;
	m_ItemBounds = new wxMenuItem( m_MenuViewHelper, wxID_ANY, wxString( _("Bounds") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewHelper->Append( m_ItemBounds );
	
	wxMenuItem* m_ItemStatistics;
	m_ItemStatistics = new wxMenuItem( m_MenuViewHelper, wxID_ANY, wxString( _("Statistics") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewHelper->Append( m_ItemStatistics );
	
	m_MenuView->Append( -1, _("Helper"), m_MenuViewHelper );
	
	m_MenuViewGeometry = new wxMenu();
	wxMenuItem* m_ItemNone;
	m_ItemNone = new wxMenuItem( m_MenuViewGeometry, wxID_ANY, wxString( _("None") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewGeometry->Append( m_ItemNone );
	
	wxMenuItem* m_ItemArt;
	m_ItemArt = new wxMenuItem( m_MenuViewGeometry, wxID_ANY, wxString( _("Art") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewGeometry->Append( m_ItemArt );
	
	wxMenuItem* m_ItemCollision;
	m_ItemCollision = new wxMenuItem( m_MenuViewGeometry, wxID_ANY, wxString( _("Collision") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewGeometry->Append( m_ItemCollision );
	
	wxMenuItem* m_ItemPathfinding;
	m_ItemPathfinding = new wxMenuItem( m_MenuViewGeometry, wxID_ANY, wxString( _("Pathfinding") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewGeometry->Append( m_ItemPathfinding );
	
	m_MenuView->Append( -1, _("Geometry"), m_MenuViewGeometry );
	
	m_MenuViewShading = new wxMenu();
	wxMenuItem* m_ItemWireframe;
	m_ItemWireframe = new wxMenuItem( m_MenuViewShading, wxID_ANY, wxString( _("Wireframe") ) + wxT('\t') + wxT("4"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShading->Append( m_ItemWireframe );
	
	wxMenuItem* m_ItemMaterial;
	m_ItemMaterial = new wxMenuItem( m_MenuViewShading, wxID_ANY, wxString( _("Material") ) + wxT('\t') + wxT("5"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShading->Append( m_ItemMaterial );
	
	wxMenuItem* m_ItemTexture;
	m_ItemTexture = new wxMenuItem( m_MenuViewShading, wxID_ANY, wxString( _("Texture") ) + wxT('\t') + wxT("6"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShading->Append( m_ItemTexture );
	
	wxMenuItem* m_separator13;
	m_separator13 = m_MenuViewShading->AppendSeparator();
	
	wxMenuItem* m_ItemWireframeOnMesh;
	m_ItemWireframeOnMesh = new wxMenuItem( m_MenuViewShading, wxID_ANY, wxString( _("Wireframe on Mesh") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShading->Append( m_ItemWireframeOnMesh );
	
	wxMenuItem* m_ItemWireframeOnShaded;
	m_ItemWireframeOnShaded = new wxMenuItem( m_MenuViewShading, wxID_ANY, wxString( _("Wireframe on Shaded") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShading->Append( m_ItemWireframeOnShaded );
	
	m_MenuView->Append( -1, _("Shading"), m_MenuViewShading );
	
	m_MenuViewCamera = new wxMenu();
	wxMenuItem* m_ItemOrbit;
	m_ItemOrbit = new wxMenuItem( m_MenuViewCamera, wxID_ANY, wxString( _("Orbit") ) + wxT('\t') + wxT("7"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewCamera->Append( m_ItemOrbit );
	
	wxMenuItem* m_ItemFront;
	m_ItemFront = new wxMenuItem( m_MenuViewCamera, wxID_ANY, wxString( _("Front") ) + wxT('\t') + wxT("8"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewCamera->Append( m_ItemFront );
	
	wxMenuItem* m_ItemSide;
	m_ItemSide = new wxMenuItem( m_MenuViewCamera, wxID_ANY, wxString( _("Side") ) + wxT('\t') + wxT("9"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewCamera->Append( m_ItemSide );
	
	wxMenuItem* m_ItemTop;
	m_ItemTop = new wxMenuItem( m_MenuViewCamera, wxID_ANY, wxString( _("Top") ) + wxT('\t') + wxT("0"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewCamera->Append( m_ItemTop );
	
	m_MenuView->Append( -1, _("Camera"), m_MenuViewCamera );
	
	m_MenuViewCulling = new wxMenu();
	wxMenuItem* m_ItemCullingFrustum;
	m_ItemCullingFrustum = new wxMenuItem( m_MenuViewCulling, wxID_ANY, wxString( _("Frustum") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewCulling->Append( m_ItemCullingFrustum );
	
	wxMenuItem* m_ItemCullingBackface;
	m_ItemCullingBackface = new wxMenuItem( m_MenuViewCulling, wxID_ANY, wxString( _("Backface") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewCulling->Append( m_ItemCullingBackface );
	
	m_MenuView->Append( -1, _("Culling"), m_MenuViewCulling );
	
	m_MenuViewShow = new wxMenu();
	wxMenuItem* m_ItemShowAll;
	m_ItemShowAll = new wxMenuItem( m_MenuViewShow, wxID_ANY, wxString( _("Show All") ) + wxT('\t') + wxT("alt-j"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShow->Append( m_ItemShowAll );
	
	wxMenuItem* m_ItemShowAllGeometry;
	m_ItemShowAllGeometry = new wxMenuItem( m_MenuViewShow, wxID_ANY, wxString( _("Show All Geometry") ) + wxT('\t') + wxT("ctrl-alt-b"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShow->Append( m_ItemShowAllGeometry );
	
	wxMenuItem* m_separator14;
	m_separator14 = m_MenuViewShow->AppendSeparator();
	
	wxMenuItem* m_ItemShowSelected;
	m_ItemShowSelected = new wxMenuItem( m_MenuViewShow, wxID_ANY, wxString( _("Show Selected") ) + wxT('\t') + wxT("alt-s"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShow->Append( m_ItemShowSelected );
	
	wxMenuItem* m_ItemShowSelectedGeometry;
	m_ItemShowSelectedGeometry = new wxMenuItem( m_MenuViewShow, wxID_ANY, wxString( _("Show Selected Geometry") ) + wxT('\t') + wxT("ctrl-b"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShow->Append( m_ItemShowSelectedGeometry );
	
	wxMenuItem* m_separator15;
	m_separator15 = m_MenuViewShow->AppendSeparator();
	
	wxMenuItem* m_ItemShowUnrelated;
	m_ItemShowUnrelated = new wxMenuItem( m_MenuViewShow, wxID_ANY, wxString( _("Show Unrelated") ) + wxT('\t') + wxT("alt-shift-s"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShow->Append( m_ItemShowUnrelated );
	
	wxMenuItem* m_separator16;
	m_separator16 = m_MenuViewShow->AppendSeparator();
	
	wxMenuItem* m_ItemShowLastHidden;
	m_ItemShowLastHidden = new wxMenuItem( m_MenuViewShow, wxID_ANY, wxString( _("Show Last Hidden") ) + wxT('\t') + wxT("ctrl-shift-h"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewShow->Append( m_ItemShowLastHidden );
	
	m_MenuView->Append( -1, _("Show"), m_MenuViewShow );
	
	m_MenuViewHide = new wxMenu();
	wxMenuItem* m_ItemHideAll;
	m_ItemHideAll = new wxMenuItem( m_MenuViewHide, wxID_ANY, wxString( _("Hide All") ) + wxT('\t') + wxT("alt-shift-j"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewHide->Append( m_ItemHideAll );
	
	wxMenuItem* m_ItemHideAllGeometry;
	m_ItemHideAllGeometry = new wxMenuItem( m_MenuViewHide, wxID_ANY, wxString( _("Hide All Geometry") ) + wxT('\t') + wxT("ctrl-alt-shift-b"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewHide->Append( m_ItemHideAllGeometry );
	
	wxMenuItem* m_separator17;
	m_separator17 = m_MenuViewHide->AppendSeparator();
	
	wxMenuItem* m_ItemHideSelected;
	m_ItemHideSelected = new wxMenuItem( m_MenuViewHide, wxID_ANY, wxString( _("Hide Selected") ) + wxT('\t') + wxT("ctrl-h"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewHide->Append( m_ItemHideSelected );
	
	wxMenuItem* m_ItemHideSelectedGeometry;
	m_ItemHideSelectedGeometry = new wxMenuItem( m_MenuViewHide, wxID_ANY, wxString( _("Hide Selected Geometry") ) + wxT('\t') + wxT("ctrl-shift-b"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewHide->Append( m_ItemHideSelectedGeometry );
	
	wxMenuItem* m_separator18;
	m_separator18 = m_MenuViewHide->AppendSeparator();
	
	wxMenuItem* m_ItemHideUnrelated;
	m_ItemHideUnrelated = new wxMenuItem( m_MenuViewHide, wxID_ANY, wxString( _("Hide Unrelated") ) + wxT('\t') + wxT("alt-h"), wxEmptyString, wxITEM_NORMAL );
	m_MenuViewHide->Append( m_ItemHideUnrelated );
	
	m_MenuView->Append( -1, _("Hide"), m_MenuViewHide );
	
	m_MenuViewColorMode = new wxMenu();
	wxMenuItem* m_ItemLayer;
	m_ItemLayer = new wxMenuItem( m_MenuViewColorMode, wxID_ANY, wxString( _("Layer") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewColorMode->Append( m_ItemLayer );
	
	wxMenuItem* m_ItemNodeType;
	m_ItemNodeType = new wxMenuItem( m_MenuViewColorMode, wxID_ANY, wxString( _("Node Type") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewColorMode->Append( m_ItemNodeType );
	
	wxMenuItem* m_ItemScene;
	m_ItemScene = new wxMenuItem( m_MenuViewColorMode, wxID_ANY, wxString( _("Scene") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewColorMode->Append( m_ItemScene );
	
	wxMenuItem* m_ItemScaleColorMode;
	m_ItemScaleColorMode = new wxMenuItem( m_MenuViewColorMode, wxID_ANY, wxString( _("Scale") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewColorMode->Append( m_ItemScaleColorMode );
	
	wxMenuItem* m_ItemScaleGradientColorMode;
	m_ItemScaleGradientColorMode = new wxMenuItem( m_MenuViewColorMode, wxID_ANY, wxString( _("Scale (Gradient)") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewColorMode->Append( m_ItemScaleGradientColorMode );
	
	m_MenuView->Append( -1, _("Color Mode"), m_MenuViewColorMode );
	
	m_MenuViewDefaults = new wxMenu();
	wxMenuItem* m_ItemShowLayers;
	m_ItemShowLayers = new wxMenuItem( m_MenuViewDefaults, wxID_ANY, wxString( _("Show Layers") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewDefaults->Append( m_ItemShowLayers );
	
	wxMenuItem* m_ItemShowInstances;
	m_ItemShowInstances = new wxMenuItem( m_MenuViewDefaults, wxID_ANY, wxString( _("Show Instances") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewDefaults->Append( m_ItemShowInstances );
	
	wxMenuItem* m_ItemShowGeometry;
	m_ItemShowGeometry = new wxMenuItem( m_MenuViewDefaults, wxID_ANY, wxString( _("Show Geometry") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewDefaults->Append( m_ItemShowGeometry );
	
	wxMenuItem* m_ItemShowPointer;
	m_ItemShowPointer = new wxMenuItem( m_MenuViewDefaults, wxID_ANY, wxString( _("Show Pointer") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewDefaults->Append( m_ItemShowPointer );
	
	wxMenuItem* m_ItemShowBounds;
	m_ItemShowBounds = new wxMenuItem( m_MenuViewDefaults, wxID_ANY, wxString( _("Show Bounds") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuViewDefaults->Append( m_ItemShowBounds );
	
	m_MenuView->Append( -1, _("Defaults"), m_MenuViewDefaults );
	
	wxMenuItem* m_ItemFrameOrigin;
	m_ItemFrameOrigin = new wxMenuItem( m_MenuView, wxID_ANY, wxString( _("Frame Origin") ) + wxT('\t') + wxT("o"), wxEmptyString, wxITEM_NORMAL );
	m_MenuView->Append( m_ItemFrameOrigin );
	
	wxMenuItem* m_ItemFrameSelected;
	m_ItemFrameSelected = new wxMenuItem( m_MenuView, wxID_ANY, wxString( _("Frame Selected") ) + wxT('\t') + wxT("f"), wxEmptyString, wxITEM_NORMAL );
	m_MenuView->Append( m_ItemFrameSelected );
	
	wxMenuItem* m_ItemHighlightMode;
	m_ItemHighlightMode = new wxMenuItem( m_MenuView, wxID_ANY, wxString( _("Highlight Mode") ) + wxT('\t') + wxT("h"), wxEmptyString, wxITEM_NORMAL );
	m_MenuView->Append( m_ItemHighlightMode );
	
	wxMenuItem* m_ItemPreviousView;
	m_ItemPreviousView = new wxMenuItem( m_MenuView, wxID_ANY, wxString( _("Previous View") ) + wxT('\t') + wxT("["), wxEmptyString, wxITEM_NORMAL );
	m_MenuView->Append( m_ItemPreviousView );
	
	wxMenuItem* m_ItemNextView;
	m_ItemNextView = new wxMenuItem( m_MenuView, wxID_ANY, wxString( _("Next View") ) + wxT('\t') + wxT("]"), wxEmptyString, wxITEM_NORMAL );
	m_MenuView->Append( m_ItemNextView );
	
	m_MainMenuBar->Append( m_MenuView, _("View") ); 
	
	m_MenuTools = new wxMenu();
	wxMenuItem* m_ItemSelect;
	m_ItemSelect = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Select") ) + wxT('\t') + wxT("q"), wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemSelect );
	
	wxMenuItem* m_ItemTranslate;
	m_ItemTranslate = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Translate") ) + wxT('\t') + wxT("w"), wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemTranslate );
	
	wxMenuItem* m_ItemRotate;
	m_ItemRotate = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Rotate") ) + wxT('\t') + wxT("e"), wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemRotate );
	
	wxMenuItem* m_ItemScale;
	m_ItemScale = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Scale") ) + wxT('\t') + wxT("r"), wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemScale );
	
	wxMenuItem* m_ItemMovePivot;
	m_ItemMovePivot = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Move Pivot") ) + wxT('\t') + wxT("t"), wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemMovePivot );
	
	wxMenuItem* m_ItemDuplicateTool;
	m_ItemDuplicateTool = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Duplicate Tool") ) + wxT('\t') + wxT("ALT-d"), wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemDuplicateTool );
	
	wxMenuItem* m_separator19;
	m_separator19 = m_MenuTools->AppendSeparator();
	
	wxMenuItem* m_ItemCreateEntity;
	m_ItemCreateEntity = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Create Entity") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemCreateEntity );
	
	wxMenuItem* m_ItemCreateLocator;
	m_ItemCreateLocator = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Create Locator") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemCreateLocator );
	
	wxMenuItem* m_ItemCreateVolume;
	m_ItemCreateVolume = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Create Volume") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemCreateVolume );
	
	wxMenuItem* m_ItemCreateCurve;
	m_ItemCreateCurve = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Create Curve") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemCreateCurve );
	
	wxMenuItem* m_ItemEditCurve;
	m_ItemEditCurve = new wxMenuItem( m_MenuTools, wxID_ANY, wxString( _("Edit Curve") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuTools->Append( m_ItemEditCurve );
	
	m_MainMenuBar->Append( m_MenuTools, _("Tools") ); 
	
	m_MenuPanels = new wxMenu();
	m_MainMenuBar->Append( m_MenuPanels, _("Panels") ); 
	
	m_MenuSettings = new wxMenu();
	wxMenuItem* m_ItemPreferences;
	m_ItemPreferences = new wxMenuItem( m_MenuSettings, wxID_ANY, wxString( _("Preferences...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuSettings->Append( m_ItemPreferences );
	
	m_MainMenuBar->Append( m_MenuSettings, _("Settings") ); 
	
	m_MenuHelp = new wxMenu();
	wxMenuItem* m_ItemAbout;
	m_ItemAbout = new wxMenuItem( m_MenuHelp, ID_About, wxString( _("About...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuHelp->Append( m_ItemAbout );
	
	m_MainMenuBar->Append( m_MenuHelp, _("Help") ); 
	
	this->SetMenuBar( m_MainMenuBar );
	
	m_MainToolbar = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY ); 
	m_MainToolbar->AddTool( ID_NewScene, _("New"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_MainToolbar->AddTool( ID_Open, _("Open"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_MainToolbar->AddTool( ID_SaveAll, _("Save"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_MainToolbar->AddSeparator(); 
	m_MainToolbar->AddTool( ID_Cut, _("Cut"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_MainToolbar->AddTool( ID_Copy, _("Copy"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_MainToolbar->AddTool( ID_Paste, _("Paste"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_MainToolbar->AddSeparator(); 
	m_MainToolbar->AddTool( ID_Undo, _("Undo"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_MainToolbar->AddTool( ID_Redo, _("Redo"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_MainToolbar->AddSeparator(); 
	m_ToolVaultSearch = new wxSearchCtrl( m_MainToolbar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 400,-1 ), 0 );
	#ifndef __WXMAC__
	m_ToolVaultSearch->ShowSearchButton( true );
	#endif
	m_ToolVaultSearch->ShowCancelButton( false );
	m_ToolVaultSearch->SetMinSize( wxSize( 200,-1 ) );
	
	m_MainToolbar->AddControl( m_ToolVaultSearch );
	m_MainToolbar->Realize();
	
	m_MainStatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	this->Connect( ID_NewScene, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNew ) );
	this->Connect( ID_NewEntity, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNew ) );
	this->Connect( ID_NewProject, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNew ) );
	this->Connect( ID_NewScene, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrameGenerated::OnNew ) );
	this->Connect( ID_Open, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrameGenerated::OnOpen ) );
	this->Connect( ID_SaveAll, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrameGenerated::OnSaveAll ) );
}

MainFrameGenerated::~MainFrameGenerated()
{
	// Disconnect Events
	this->Disconnect( ID_NewScene, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNew ) );
	this->Disconnect( ID_NewEntity, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNew ) );
	this->Disconnect( ID_NewProject, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNew ) );
	this->Disconnect( ID_NewScene, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrameGenerated::OnNew ) );
	this->Disconnect( ID_Open, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrameGenerated::OnOpen ) );
	this->Disconnect( ID_SaveAll, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrameGenerated::OnSaveAll ) );
	
}

HelpPanelGenerated::HelpPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 300,200 ) );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_HelpRichText = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
	bSizer32->Add( m_HelpRichText, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer32 );
	this->Layout();
}

HelpPanelGenerated::~HelpPanelGenerated()
{
}

LayersPanelGenerated::LayersPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	m_LayersToolbar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	m_LayersToolbar->AddTool( ID_CreateNewLayer, _("Create New Layer"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_LayersToolbar->AddTool( ID_CreateNewLayerFromSelection, _("Create New Layer From Selection"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_LayersToolbar->AddTool( ID_DeleteSelectedLayers, _("Delete Selected Layers"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_LayersToolbar->AddSeparator(); 
	m_LayersToolbar->AddTool( ID_AddSelectionToLayers, _("Add Selection to Selected Layers"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_LayersToolbar->AddTool( ID_RemoveSelectionFromLayers, _("Remove Selection from Selected Layers"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_LayersToolbar->AddSeparator(); 
	m_LayersToolbar->AddTool( ID_SelectLayerMembers, _("Select Layer Members"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_LayersToolbar->AddTool( ID_SelectLayers, _("Select Layers"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_LayersToolbar->Realize();
	
	bSizer19->Add( m_LayersToolbar, 0, wxEXPAND, 5 );
	
	m_LayersPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer19->Add( m_LayersPanel, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer19 );
	this->Layout();
}

LayersPanelGenerated::~LayersPanelGenerated()
{
}

VaultPanelGenerated::VaultPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );
	
	m_NavigationPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxHORIZONTAL );
	
	m_BackButton = new wxBitmapButton( m_NavigationPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer34->Add( m_BackButton, 0, wxALL, 5 );
	
	m_ForwardButton = new wxBitmapButton( m_NavigationPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer34->Add( m_ForwardButton, 0, wxALL, 5 );
	
	m_SearchCtrl = new wxSearchCtrl( m_NavigationPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	m_SearchCtrl->ShowSearchButton( true );
	#endif
	m_SearchCtrl->ShowCancelButton( false );
	bSizer34->Add( m_SearchCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_OptionsButton = new wxBitmapButton( m_NavigationPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer34->Add( m_OptionsButton, 0, wxALL, 5 );
	
	m_NavigationPanel->SetSizer( bSizer34 );
	m_NavigationPanel->Layout();
	bSizer34->Fit( m_NavigationPanel );
	bSizer33->Add( m_NavigationPanel, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	m_SourceListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_SourceListBox->Append( _("Local") );
	m_SourceListBox->Append( _("TurboSquid") );
	bSizer24->Add( m_SourceListBox, 0, wxALL|wxEXPAND, 5 );
	
	m_ResultsPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer24->Add( m_ResultsPanel, 1, wxEXPAND | wxALL, 5 );
	
	bSizer33->Add( bSizer24, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer33 );
	this->Layout();
}

VaultPanelGenerated::~VaultPanelGenerated()
{
}

DetailsPanelGenerated::DetailsPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 300,350 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_ScrollWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* scrollSizer;
	scrollSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizer1;
	sizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelName = new wxStaticText( m_ScrollWindow, wxID_ANY, _("Name"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelName->Wrap( -1 );
	sizer1->Add( m_LabelName, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Name = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_Name->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer1->Add( m_Name, 1, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	
	scrollSizer->Add( sizer1, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizer2;
	sizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelFileType = new wxStaticText( m_ScrollWindow, wxID_ANY, _("File Type"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelFileType->Wrap( -1 );
	sizer2->Add( m_LabelFileType, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_FileType = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_FileType->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer2->Add( m_FileType, 1, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	
	scrollSizer->Add( sizer2, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizer3;
	sizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelFileID = new wxStaticText( m_ScrollWindow, wxID_ANY, _("File ID"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelFileID->Wrap( -1 );
	sizer3->Add( m_LabelFileID, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_FileID = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_FileID->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer3->Add( m_FileID, 1, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	
	scrollSizer->Add( sizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizer4;
	sizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_LabelFolder = new wxStaticText( m_ScrollWindow, wxID_ANY, _("Folder"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelFolder->Wrap( -1 );
	sizer4->Add( m_LabelFolder, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 5 );
	
	m_Folder = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_Folder->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer4->Add( m_Folder, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 2 );
	
	scrollSizer->Add( sizer4, 0, wxBOTTOM|wxEXPAND, 5 );
	
	wxBoxSizer* sizer5;
	sizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_RevisionPanel = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* revisionSizer;
	revisionSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LabelPerforce = new wxStaticText( m_RevisionPanel, wxID_ANY, _("Perforce"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelPerforce->Wrap( -1 );
	m_LabelPerforce->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	revisionSizer->Add( m_LabelPerforce, 0, wxALL, 5 );
	
	wxBoxSizer* statusSizer;
	statusSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelRevisionStatus = new wxStaticText( m_RevisionPanel, wxID_ANY, _("Status"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelRevisionStatus->Wrap( -1 );
	statusSizer->Add( m_LabelRevisionStatus, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_StatusPanel = new wxPanel( m_RevisionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* statusInnerSizer;
	statusInnerSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_RevisionStatusIcon = new wxStaticBitmap( m_StatusPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 16,16 ), 0 );
	statusInnerSizer->Add( m_RevisionStatusIcon, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 1 );
	
	m_RevisionStatus = new wxStaticText( m_StatusPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_RevisionStatus->Wrap( -1 );
	statusInnerSizer->Add( m_RevisionStatus, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_StatusPanel->SetSizer( statusInnerSizer );
	m_StatusPanel->Layout();
	statusInnerSizer->Fit( m_StatusPanel );
	statusSizer->Add( m_StatusPanel, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 2 );
	
	revisionSizer->Add( statusSizer, 0, wxEXPAND, 5 );
	
	m_LastCheckInPanel = new wxPanel( m_RevisionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* lastCheckinSizer;
	lastCheckinSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LabelLastCheckIn = new wxStaticText( m_LastCheckInPanel, wxID_ANY, _("Last Check In"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelLastCheckIn->Wrap( -1 );
	lastCheckinSizer->Add( m_LabelLastCheckIn, 0, wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* innerCheckinSizer;
	innerCheckinSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LastCheckIn = new wxTextCtrl( m_LastCheckInPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,75 ), wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
	m_LastCheckIn->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	innerCheckinSizer->Add( m_LastCheckIn, 0, wxALL|wxEXPAND, 2 );
	
	lastCheckinSizer->Add( innerCheckinSizer, 1, wxEXPAND, 5 );
	
	m_LastCheckInPanel->SetSizer( lastCheckinSizer );
	m_LastCheckInPanel->Layout();
	lastCheckinSizer->Fit( m_LastCheckInPanel );
	revisionSizer->Add( m_LastCheckInPanel, 1, wxEXPAND, 5 );
	
	m_FirstCheckInPanel = new wxPanel( m_RevisionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* firstCheckInSizer;
	firstCheckInSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LabelFirstCheckIn = new wxStaticText( m_FirstCheckInPanel, wxID_ANY, _("First Check In"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelFirstCheckIn->Wrap( -1 );
	firstCheckInSizer->Add( m_LabelFirstCheckIn, 0, wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* innerFirstCheckInSizer;
	innerFirstCheckInSizer = new wxBoxSizer( wxVERTICAL );
	
	m_FirstCheckIn = new wxTextCtrl( m_FirstCheckInPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,75 ), wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
	m_FirstCheckIn->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	innerFirstCheckInSizer->Add( m_FirstCheckIn, 0, wxALL|wxEXPAND, 2 );
	
	firstCheckInSizer->Add( innerFirstCheckInSizer, 1, wxEXPAND, 5 );
	
	m_FirstCheckInPanel->SetSizer( firstCheckInSizer );
	m_FirstCheckInPanel->Layout();
	firstCheckInSizer->Fit( m_FirstCheckInPanel );
	revisionSizer->Add( m_FirstCheckInPanel, 1, wxEXPAND, 5 );
	
	m_RevisionPanel->SetSizer( revisionSizer );
	m_RevisionPanel->Layout();
	revisionSizer->Fit( m_RevisionPanel );
	sizer5->Add( m_RevisionPanel, 0, wxEXPAND, 5 );
	
	scrollSizer->Add( sizer5, 0, wxEXPAND, 5 );
	
	m_ScrollWindow->SetSizer( scrollSizer );
	m_ScrollWindow->Layout();
	scrollSizer->Fit( m_ScrollWindow );
	mainSizer->Add( m_ScrollWindow, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

DetailsPanelGenerated::~DetailsPanelGenerated()
{
}

ToolsPanelGenerated::ToolsPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxVERTICAL );
	
	m_ToolsContainerPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer28->Add( m_ToolsContainerPanel, 0, wxEXPAND, 5 );
	
	m_PropertiesPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer28->Add( m_PropertiesPanel, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer28 );
	this->Layout();
}

ToolsPanelGenerated::~ToolsPanelGenerated()
{
}

TypesPanelGenerated::TypesPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
}

TypesPanelGenerated::~TypesPanelGenerated()
{
}

ProjectPanelGenerated::ProjectPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	m_DataViewCtrl = new wxDataViewCtrl ( this, wxID_ANY );
	bSizer24->Add( m_DataViewCtrl, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer24 );
	this->Layout();
}

ProjectPanelGenerated::~ProjectPanelGenerated()
{
}

DirectoryPanelGenerated::DirectoryPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	m_DirectoryNotebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_HierarchyPanel = new wxPanel( m_DirectoryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_DirectoryNotebook->AddPage( m_HierarchyPanel, _("Hierarchy"), false );
	m_EntitiesPanel = new wxPanel( m_DirectoryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_DirectoryNotebook->AddPage( m_EntitiesPanel, _("Entities"), false );
	m_TypesPanel = new wxPanel( m_DirectoryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_DirectoryNotebook->AddPage( m_TypesPanel, _("Types"), false );
	
	bSizer21->Add( m_DirectoryNotebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer21 );
	this->Layout();
}

DirectoryPanelGenerated::~DirectoryPanelGenerated()
{
}

PropertiesPanelGenerated::PropertiesPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
}

PropertiesPanelGenerated::~PropertiesPanelGenerated()
{
}

ViewPanelGenerated::ViewPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	m_ToolbarView = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	m_ToolbarView->AddTool( ID_CameraOrbit, _("Orbit"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_ToolbarView->AddTool( ID_CameraFront, _("Front"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_ToolbarView->AddTool( ID_CameraSide, _("Side"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_ToolbarView->AddTool( ID_CameraTop, _("Top"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_ToolbarView->Realize();
	
	bSizer21->Add( m_ToolbarView, 0, wxEXPAND, 5 );
	
	m_ViewPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* m_ViewSizer;
	m_ViewSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ViewPanel->SetSizer( m_ViewSizer );
	m_ViewPanel->Layout();
	m_ViewSizer->Fit( m_ViewPanel );
	bSizer21->Add( m_ViewPanel, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer21 );
	this->Layout();
}

ViewPanelGenerated::~ViewPanelGenerated()
{
}
