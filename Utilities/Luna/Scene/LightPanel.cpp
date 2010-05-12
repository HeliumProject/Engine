#include "Precompile.h"
#include "LightPanel.h"

#include "AttributeOverrideGroup.h"
#include "Light.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Editor/SessionManager.h"

#include "Asset/LevelAsset.h"
#include "Asset/CubeMapAttribute.h"
#include "math/Color3.h"

#include "Content/StencilTextureAttribute.h"
#include "Attribute/AttributeHandle.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
//
LightPanel::LightPanel( Enumerator* enumerator, const OS_SelectableDumbPtr& selection )
: m_Selection( selection )
{
  m_Interpreter = m_Enumerator = enumerator;
  m_Text = "Light";
  m_Expanded = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Creates the panel.
//
void LightPanel::Create()
{  
  //Create the interpreter
  m_ReflectInterpreter  = m_Enumerator->CreateInterpreter< Inspect::ReflectInterpreter >( this );

  AttributeOverrideGroup< Content::StencilTextureAttribute, Light >* stencilTexPanel = 
    new AttributeOverrideGroup< Content::StencilTextureAttribute, Light >( "Stencil Texture", m_Enumerator, m_Selection, 
    &Light::GetOverride< Content::StencilTextureAttribute >, 
    &Light::SetOverride< Content::StencilTextureAttribute > );

  m_Enumerator->Push( stencilTexPanel );
  {
    stencilTexPanel->SetCanvas( m_Enumerator->GetContainer()->GetCanvas() );
    stencilTexPanel->Create();
    m_Enumerator->AddLabel("Show Pointer");
  }
  m_Enumerator->Pop(); 

  Inspect::Panel::Create();
}