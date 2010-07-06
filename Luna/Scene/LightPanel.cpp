#include "Precompile.h"
#include "LightPanel.h"

#include "Light.h"
#include "Scene.h"
#include "SceneManager.h"

#include "Pipeline/Asset/Classes/SceneAsset.h"
#include "Foundation/Math/Color3.h"

#include "Foundation/Component/ComponentHandle.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
//
LightPanel::LightPanel( Enumerator* enumerator, const OS_SelectableDumbPtr& selection )
: m_Selection( selection )
{
  m_Interpreter = m_Enumerator = enumerator;
  m_Text = TXT( "Light" );
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
  Inspect::Panel::Create();
}