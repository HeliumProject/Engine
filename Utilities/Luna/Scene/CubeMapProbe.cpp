#include "Precompile.h"
#include "CubeMapProbe.h"

#include "Scene.h"
#include "SceneManager.h"
#include "InstanceType.h"

#include "Core/Enumerator.h"
#include "UIToolKit/ImageManager.h"

#include "PrimitiveSphere.h"
#include "PrimitivePointer.h"

#include "File/Manager.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::CubeMapProbe);

void CubeMapProbe::InitializeType()
{
  Reflect::RegisterClass< Luna::CubeMapProbe >( "Luna::CubeMapProbe" );

  Enumerator::InitializePanel( "CubeMapProbe", CreatePanelSignature::Delegate( &CubeMapProbe::CreatePanel ) );
}

void CubeMapProbe::CleanupType()
{
  Reflect::UnregisterClass< Luna::CubeMapProbe >();
}

CubeMapProbe::CubeMapProbe(Luna::Scene* scene)
: Luna::Instance ( scene, new Content::CubeMapProbe() )
{

}

CubeMapProbe::CubeMapProbe(Luna::Scene* scene, Content::CubeMapProbe* sky )
: Luna::Instance ( scene, sky )
{

}

i32 CubeMapProbe::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "cube_16.png" );
}

std::string CubeMapProbe::GetApplicationTypeName() const
{
  return "CubeMapProbe";
}

void CubeMapProbe::Evaluate(GraphDirection direction)
{
  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      // start the box from scratch
      m_ObjectBounds.Reset();

      // merge type pointer into our bounding box
      if (m_NodeType)
      {
        Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>(m_NodeType);
        m_ObjectBounds.Merge( type->GetPointer()->GetBounds() );
      }
      break;
    }
  }

  __super::Evaluate(direction);
}

void CubeMapProbe::Render( RenderVisitor* render )
{
  // pointer is drawn normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix.Normalized();
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &CubeMapProbe::DrawPointer;
  }

  // don't call __super here, it will draw big ass axes
  Luna::HierarchyNode::Render( render );
}

void CubeMapProbe::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::CubeMapProbe* sky = Reflect::ConstAssertCast<Luna::CubeMapProbe>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( sky->GetNodeType() );

  sky->SetMaterial( type->GetMaterial() );

  // draw type pointer
  type->GetPointer()->Draw( args );
}

bool CubeMapProbe::Pick( PickVisitor* pick )
{
  Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>(m_NodeType);

  pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());

  if (type->GetPointer()->Pick(pick))
  {
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Luna::CubeMapProbe.
// 
bool CubeMapProbe::ValidatePanel(const std::string& name)
{
  if (name == "CubeMapProbe")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void CubeMapProbe::CreatePanel( CreatePanelArgs& args )
{
  args.m_Enumerator->PushPanel("CubeMapProbe", true);
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("CubeMapAsset");
      Inspect::Value* textBox = args.m_Enumerator->AddValue<Luna::CubeMapProbe, std::string>( args.m_Selection, &CubeMapProbe::GetCubeMapAsset, &CubeMapProbe::SetCubeMapAsset );
      //textBox->AddValidationListener( ControlValidateSignature::Delegate ( args.m_Selection, &Luna::LightingEnvironment::ValidateTexturePath ) );

      Inspect::FileDialogButton* fileDialog = args.m_Enumerator->AddFileDialogButton<Luna::CubeMapProbe, std::string>( args.m_Selection, &CubeMapProbe::GetCubeMapAsset, &CubeMapProbe::SetCubeMapAsset );
      fileDialog->SetTuidRequired( true );

      Inspect::FileBrowserButton* fileBrowser = args.m_Enumerator->AddFileBrowserButton<Luna::CubeMapProbe, std::string>( args.m_Selection, &CubeMapProbe::GetCubeMapAsset, &CubeMapProbe::SetCubeMapAsset );
      fileBrowser->SetTuidRequired( true );

    }
    args.m_Enumerator->Pop();
  }
  args.m_Enumerator->Pop();
}

const std::string& CubeMapProbe::GetCubeMapAsset() const
{
  if( !m_CubeMapAssetPath.empty() )
    return m_CubeMapAssetPath;

  tuid cubeMapID = GetPackage< Content::CubeMapProbe >()->m_CubeMapID;

  try
  {
    m_CubeMapAssetPath = File::GlobalManager().GetPath( cubeMapID );
  }
  catch ( const File::Exception& e )
  {
    Console::Error( "%s\n", e.what() );
  }

  return m_CubeMapAssetPath;
}

void CubeMapProbe::SetCubeMapAsset( const std::string& assetPath )
{  
  tuid fileID = TUID::Null;
  try
  {
    fileID = File::GlobalManager().GetID( assetPath );
  }
  catch ( const File::Exception& e )
  {
    Console::Error( "%s\n", e.what() );
    return;
  }
  Content::CubeMapProbe* probe = GetPackage< Content::CubeMapProbe >();
  probe->m_CubeMapID = fileID;
  m_CubeMapAssetPath = assetPath;
}
