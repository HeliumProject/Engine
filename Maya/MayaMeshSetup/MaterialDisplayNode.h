#pragma once

#include "API.h"

#include "Content/Material.h"
#include "Common/Memory/SmartPtr.h"

#include "Math/Color3.h"


// Forwards
namespace Content
{
  class Material;
  typedef Nocturnal::SmartPtr< Material > MaterialPtr;
}

namespace Maya
{
  //extern const char* MaterialComponentsAttrStrings[Content::MaterialComponents::Unknown];

  namespace MaterialDisplayFlags
  {
    enum MaterialDisplayFlag
    {
      // This should always be -1
      Off = -1,

      Substance = 0,
      Acoustic,
      Physics,
    };
  }
  typedef MAYAMESHSETUP_API MaterialDisplayFlags::MaterialDisplayFlag MaterialDisplayFlag;

  typedef MAYAMESHSETUP_API std::map< std::string, MObject > M_StringMObject;
  typedef MAYAMESHSETUP_API std::vector< M_StringMObject > VM_StringMObject;

  ///////////////////////////////////////////////////////////////////////////////

  struct PaintPolyPoint
  {
    double X, Y, Z;
    void Set( const double x, const double y, const double z )
    {
      X = x;
      Y = y;
      Z = z;
    }
  };
  typedef MAYAMESHSETUP_API std::vector< PaintPolyPoint > V_PaintPolyPoint;

  struct PaintPolyColor
  {
    MColor           m_DrawColor;
    V_PaintPolyPoint m_Point;
  };

  typedef MAYAMESHSETUP_API std::vector< PaintPolyColor > V_PaintPolyColor;

  ///////////////////////////////////////////////////////////////////////////////
  // 
  //
  class MAYAMESHSETUP_API MaterialDisplayNode : public MPxLocatorNode
  {
  public:
    MaterialDisplayNode();
    virtual ~MaterialDisplayNode(); 

    //
    // Plugin API
    //

    static  void*   Creator();
    static MStatus  Initialize();
    static MStatus  AddCallbacks();
    static MStatus  RemoveCallbacks();

    //
    // MPxLocatorNode API
    //

    virtual void    postConstructor();
    virtual MStatus compute( const MPlug& plug, MDataBlock& data );
    virtual void    draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status );

    //
    // Node Creation/Deletion
    //

    // Creates and adds a MaterialDisplayNode to the scene
    static MObject  GetMaterialDisplayNode( MStatus* returnStatus = 0 );

    // Removes all MaterialDisplayNodes from the scene, and data stored on associated meshes
    static MStatus  DeleteMaterialDisplayNodes();

    //
    // Assignment
    //

    // Assign the given material to selected objects
    static MStatus  AssignMaterialToSelection( const Content::MaterialPtr& material );
    static MStatus  AssignMaterialToSelection( const Content::MaterialComponent materialComponent, const std::string& component );

    // Clears all material data from the selected objects
    static MStatus  ClearMaterialsFromObjects();
    static MStatus  ClearMaterialsFromObjects( const Content::MaterialComponent materialComponent );

    //
    // Display
    //

    // Updates the display to show what materials are assigned to what objects
    static MStatus  UpdateDisplay( const MaterialDisplayFlag materialDisplayFlag );

    static MStatus  UpdateDisplayDrawPolys( MObject& node );

    // Generates a Math::Color3 based on the hash of the given string
    static Math::Color3 GetColorFromMaterialComponent( const std::string& component );

    
    //
    // Selection
    //

    // Select the objects in the scene that are assigned with the given material
    static MStatus  SelectObjectsWithMaterial( const Content::MaterialPtr& material );
    static MStatus  SelectObjectsWithMaterial( const Content::MaterialComponent materialComponent, const std::string& component );

    static Content::MaterialPtr GetFaceMaterial( const MFnMesh& meshFn, const i32 faceIndex, MStatus* returnStatus = NULL );
    static Content::MaterialPtr GetObjectMaterial( const MObject& object, MStatus* returnStatus );

    //
    // Callbacks
    //

    static void     NodeAddedCallback( MObject& node, void* clientData );
    static void     NodeRemovedCallback( MObject& node, void* clientData );
    static void     FlushCallback( void* clientData );

  private:

    //
    // Assignment
    //

    // Creates the material blind data template for the polys
    static MStatus  CreateMaterialBlindData( MFnMesh& meshFn );

    // Get/Set the Material blind data on mesh face
    static MStatus  SetFaceMaterial( MFnMesh& meshFn, const i32 faceIndex, const Content::MaterialPtr& material );
    static MStatus  SetObjectMaterial( MObject& object, const Content::MaterialPtr& material );
    
    static MStatus  RemoveObjectMaterial( MObject& object );

    //static MObject  GetMaterialComponentAttr( MObject& object, const Content::MaterialComponent materialComponent, const std::string& componentName, MStatus* returnStatus = 0 );
    //static MStatus  ConnectMaterialComponentAttr( MObject& object, MPlug& nodeAttrPlug, const Content::MaterialComponent materialComponent, const std::string& componentName, bool connect = true );

    //
    // Display
    //

    //
    // Selection
    //

  public: 

    static MTypeId    m_NodeId;
    static MString    m_NodeName;

    // callbacks
    static MIntArray  m_CallbackIDs;

    // attributes
    static MObject    m_DisplayFlagAttr;
    static MString    m_DisplayFlagAttrName;

    // things to make drawing faster
    static MaterialDisplayFlag m_DisplayFlag;
    static V_PaintPolyColor m_PaintPolyColorVec;


    //static VM_StringMObject m_MaterialComponentAttrbutes;
  };

} // namespace Maya