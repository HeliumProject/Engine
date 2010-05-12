#include "Precompile.h"
#include "LightingEnvironment.h"

#include "Scene.h"
#include "SceneManager.h"
#include "InstanceType.h"

#include "Core/Enumerator.h"
#include "UIToolKit/ImageManager.h"

#include "PrimitiveSphere.h"
#include "PrimitivePointer.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::LightingEnvironment);

void LightingEnvironment::InitializeType()
{
  Reflect::RegisterClass< Luna::LightingEnvironment >( "Luna::LightingEnvironment" );

  Enumerator::InitializePanel( "LightingEnvironment", CreatePanelSignature::Delegate( &LightingEnvironment::CreatePanel ) );
}

void LightingEnvironment::CleanupType()
{
  Reflect::UnregisterClass< Luna::LightingEnvironment >();
}

LightingEnvironment::LightingEnvironment(Luna::Scene* scene)
: Luna::Instance ( scene, new Content::LightingEnvironment() )
{

}

LightingEnvironment::LightingEnvironment(Luna::Scene* scene, Content::LightingEnvironment* sky )
: Luna::Instance ( scene, sky )
{

}

i32 LightingEnvironment::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "light_16.png" );
}

std::string LightingEnvironment::GetApplicationTypeName() const
{
  return "LightingEnvironment";
}

void LightingEnvironment::Evaluate(GraphDirection direction)
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

void LightingEnvironment::Render( RenderVisitor* render )
{
  // pointer is drawn normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix.Normalized();
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &LightingEnvironment::DrawPointer;
  }

  // don't call __super here, it will draw big ass axes
  Luna::HierarchyNode::Render( render );
}

void LightingEnvironment::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::LightingEnvironment* sky = Reflect::ConstAssertCast<Luna::LightingEnvironment>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( sky->GetNodeType() );

  sky->SetMaterial( type->GetMaterial() );

  // draw type pointer
  type->GetPointer()->Draw( args );
}

bool LightingEnvironment::Pick( PickVisitor* pick )
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
// Returns true if the specified panel is supported by Luna::LightingEnvironment.
// 
bool LightingEnvironment::ValidatePanel(const std::string& name)
{
  if (name == "LightingEnvironment")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void LightingEnvironment::CreatePanel( CreatePanelArgs& args )
{
  args.m_Enumerator->PushPanel("Lighting Environment", true);
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Texture");
      Inspect::Value* textBox = args.m_Enumerator->AddValue<Luna::LightingEnvironment, std::string>( args.m_Selection, &LightingEnvironment::GetTexture, &LightingEnvironment::SetTexture );
      //textBox->AddValidationListener( ControlValidateSignature::Delegate ( args.m_Selection, &LightingEnvironment::ValidateTexturePath ) );

      Inspect::FileDialogButton* fileDialog = args.m_Enumerator->AddFileDialogButton<Luna::LightingEnvironment, std::string>( args.m_Selection, &LightingEnvironment::GetTexture, &LightingEnvironment::SetTexture );
      fileDialog->SetTuidRequired( true );
      
      Inspect::FileBrowserButton* fileBrowser = args.m_Enumerator->AddFileBrowserButton<Luna::LightingEnvironment, std::string>( args.m_Selection, &LightingEnvironment::GetTexture, &LightingEnvironment::SetTexture );
      fileBrowser->SetTuidRequired( true );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Intensity");
      args.m_Enumerator->AddValue<Luna::LightingEnvironment, float>( args.m_Selection, &LightingEnvironment::GetIntensity, &LightingEnvironment::SetIntensity );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Color");
      args.m_Enumerator->AddValue<Luna::LightingEnvironment, float>( args.m_Selection, &LightingEnvironment::GetRed,   &LightingEnvironment::SetRed );
      args.m_Enumerator->AddValue<Luna::LightingEnvironment, float>( args.m_Selection, &LightingEnvironment::GetGreen, &LightingEnvironment::SetGreen );
      args.m_Enumerator->AddValue<Luna::LightingEnvironment, float>( args.m_Selection, &LightingEnvironment::GetBlue,  &LightingEnvironment::SetBlue );
    }
    args.m_Enumerator->Pop();
  }
  args.m_Enumerator->Pop();
}

const std::string& LightingEnvironment::GetTexture() const
{
  return GetPackage< Content::LightingEnvironment >()->GetEnvironmentTexturePath();
}

void LightingEnvironment::SetTexture( const std::string& texturePath )
{  
  tuid fileID = TUID::Null;
  try
  {
    fileID = File::GlobalManager().GetID( texturePath );
  }
  catch ( const File::Exception& e )
  {
    Console::Error( "%s\n", e.what() );
    return;
  }
  GetPackage< Content::LightingEnvironment >()->m_TextureID = fileID;

}

f32 LightingEnvironment::GetIntensity() const
{
  return GetPackage< Content::LightingEnvironment >()->m_Intensity;
}

void LightingEnvironment::SetIntensity( f32 intensity )
{
  GetPackage< Content::LightingEnvironment >()->m_Intensity = intensity;
}

float LightingEnvironment::GetRed() const
{
  return GetPackage< Content::LightingEnvironment >()->m_Color.x;
}
void LightingEnvironment::SetRed( float color )
{
  GetPackage< Content::LightingEnvironment >()->m_Color.x = color;
}

float LightingEnvironment::GetGreen() const
{
  return GetPackage< Content::LightingEnvironment >()->m_Color.y;
}
void LightingEnvironment::SetGreen( float color )
{
  GetPackage< Content::LightingEnvironment >()->m_Color.y = color;
}

float LightingEnvironment::GetBlue() const
{
  return GetPackage< Content::LightingEnvironment >()->m_Color.z;
}
void LightingEnvironment::SetBlue( float color )
{
  GetPackage< Content::LightingEnvironment >()->m_Color.z = color;
}

bool LightingEnvironment::ValidateTexturePath( const Inspect::DataChangingArgs& args )
{
  bool result = false;

  std::string newValue;
  Reflect::Serializer::GetValue(args.m_NewValue, newValue);
  try
  {
    // Make sure the file exists on disc
    if ( FileSystem::Exists( newValue ) )
    {
      // Make sure the file can be resolved to a TUID
      if ( File::GlobalManager().GetID( newValue ) != TUID::Null )
      {
        result = true;
      }
    }
  }
  catch ( const Nocturnal::Exception& )
  {
    result = false;
  }

  if ( !result )
  {
    // Message to the user that the value is not correct.
   // wxWindow* window = Inspect::Control::GetWindow();
   // wxMessageBox( "Invalid Texture Path specified!", "Error", wxOK | wxCENTER | wxICON_ERROR, window  );
  }

  return result;
}

Luna::LightingEnvironment* LightingEnvironment::GetDefaultLightingEnvironment( Luna::Scene* currentScene )
{
  Luna::LightingEnvironment* env = NULL;
  Luna::Scene* rootScene = currentScene->GetManager()->GetRootScene();
  if( rootScene )
  {
    V_LightingEnvironmentDumbPtr environments;
    rootScene->GetAll< Luna::LightingEnvironment >( environments );

    // return the first existing Luna::LightingEnvironment
    if( environments.size() )
    {
      env = environments[0];   
    }
  }
  return env;
}